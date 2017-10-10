#include "jenkinsrequest.h"
#include "jenkinsmanager.h"
#include "../NotificationManager/notificationmanager.h"
#include "../../Utils/processrunner.h"

#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrlQuery>
#include <QMutexLocker>

#define TIME_OUT_MS 100

JenkinsRequest::JenkinsRequest(JenkinsManager *jenkins_manager, QObject *parent) : QObject()
{
    {
        QThread* thread = new QThread();
        thread->start();
        //Move request to Thread
        moveToThread(thread);

        //Cleanup Thread once this Request is dead
        connect(this, &QObject::destroyed, thread, &QThread::quit);
        //Delete Thread once finished
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    }

    jenkins_manager_ = jenkins_manager;

    //Connect destroyed signals from parents to our Terminate signal
    connect(jenkins_manager_, &JenkinsManager::destroyed, this, &JenkinsRequest::Terminate_, Qt::QueuedConnection);
    if(parent){
        connect(parent, &QObject::destroyed, this, &JenkinsRequest::Terminate_, Qt::QueuedConnection);
    }

    //Connect Terminate signal to teardown and cancel the process runner
    connect(this, &JenkinsRequest::Terminate_, this, &JenkinsRequest::Teardown_, Qt::QueuedConnection);

    //Clean up after finished
    connect(this, &JenkinsRequest::Finished, this, &QObject::deleteLater, Qt::QueuedConnection);
}


JenkinsRequest::~JenkinsRequest()
{
    QMutexLocker(&this->mutex_);
    if(jenkins_manager_){
        jenkins_manager_->jenkinsRequestFinished(this);
    }
}


bool JenkinsRequest::BlockUntilValidatedSettings()
{
    bool got_valid_settings = gotValidatedSettings();

    if(!got_valid_settings){
        QEventLoop waitLoop;

        connect(this, &JenkinsRequest::Terminate_, &waitLoop, &QEventLoop::quit);
        {
            QMutexLocker(&this->mutex_);
            if(jenkins_manager_){
                //Validated settings should terminate the loop
                connect(jenkins_manager_, &JenkinsManager::gotValidSettings, this, [&waitLoop](bool, QString){waitLoop.quit();});
            }
        }
        waitLoop.exec();
        got_valid_settings = gotValidatedSettings();
    }
    return got_valid_settings;
}

QJsonDocument JenkinsRequest::_getJobConfiguration(QString jobName, int buildNumber, QString activeConfiguration, bool refresh)
{
    QString cached_name = jobName;

    //If we have been provided a valid activeConfiguration, append this to the jobName
    if(activeConfiguration != ""){
        cached_name += "/" + activeConfiguration;
    }

    //If we have been provided a valid build number, append this to the jobName
    if(buildNumber >= 0){
        cached_name += "/" + QString::number(buildNumber);
    }

    QJsonDocument configuration;

    {
        //Get a cached copy
        QMutexLocker(&this->mutex_);
        configuration = jenkins_manager_->getJobConfiguration(cached_name);
    }

    if(configuration.isNull() || refresh){
        QString config_url = getURL() + "job/" + cached_name + "/api/json";
        auto config_request = getAuthenticatedRequest(config_url);
        auto config_result = GetRunner()->HTTPGet(config_request);

        if(config_result.success){
            //Try and load the configuration
            configuration = QJsonDocument::fromJson(config_result.standard_output.toUtf8());
            if(!configuration.isNull()){
                QMutexLocker(&this->mutex_);
                jenkins_manager_->storeJobConfiguration(cached_name, configuration);
            }
        }
    }

    return configuration;
}

void JenkinsRequest::GetJobParameters(QString job_name)
{
     if(BlockUntilValidatedSettings()){
        Jenkins_JobParameters jobParameters;

        //Get the current configuration for the Job.
        QJsonDocument configuration = _getJobConfiguration(job_name);

        if(!configuration.isNull()){
            QJsonObject configData = configuration.object();
            QString job_type = configData["_class"].toString();

            QJsonArray parameters;

            if(job_type == "hudson.matrix.MatrixProject"){
                //Get the parameterDefinitions Array from the actions Array.
                auto actions_array = configData["actions"].toArray();

                for(QJsonValue action: actions_array){
                    auto action_class = action.toObject()["_class"];
                    if(action_class.toString() == "hudson.model.ParametersDefinitionProperty"){
                        parameters = action.toObject()["parameterDefinitions"].toArray();
                        break;
                    }
                }
            }else if(job_type == "org.jenkinsci.plugins.workflow.job.WorkflowJob"){
                //Get the parameterDefinitions Array from the actions Array.
                auto property_array = configData["property"].toArray();

                for(QJsonValue property: property_array){
                    auto action_class = property.toObject()["_class"];
                    if(action_class.toString() == "hudson.model.ParametersDefinitionProperty"){
                        parameters = property.toObject()["parameterDefinitions"].toArray();
                        break;
                    }
                }
            }else{
                qCritical() << "Can't handle Job of Type: " << job_type;
            }


            //For each parameter in the parameters Array, add it to the returnable list.
            foreach(QJsonValue parameter, parameters){
                QJsonObject parameterData = parameter.toObject();

                Jenkins_Job_Parameter jobParameter;

                //Fill the data in the Jenkins_Job_Parameter from the JSON object.
                jobParameter.name = parameterData["name"].toString();
                //The type is in format [type]ParameterDefinition, so trim this.
                jobParameter.type = parameterData["type"].toString().replace("ParameterDefinition","");
                jobParameter.description = parameterData["description"].toString();
                QJsonValue defaultValue = parameterData["defaultParameterValue"].toObject()["value"];
                if(defaultValue.isBool()){
                    if(defaultValue.toBool()){
                        jobParameter.defaultValue = "true";
                    }else{
                        jobParameter.defaultValue = "false";
                    }
                }else{
                    jobParameter.defaultValue = defaultValue.toString();
                }
                //Add it the jobParameter List
                jobParameters.append(jobParameter);
            }
        }

        //Emit the SIGNAL to send the data back.
        emit GotJobParameters(job_name, jobParameters);
    }

    //Emit the SIGNAL to state the JenkinsRequest
    emit Finished();
}

void JenkinsRequest::RunGroovyScript(QString groovy_script)
{
    bool success = false;
    QString result;

    if(BlockUntilValidatedSettings()){
        QUrlQuery script;
        //Encode the script
        script.addQueryItem("script", QUrl::toPercentEncoding(groovy_script));

        auto script_url = getURL() + "scriptText";
        auto script_request = getAuthenticatedRequest(script_url);
        auto script_result = GetRunner()->HTTPPost(script_request, script.toString().toUtf8());

        success = script_result.success;
        result = script_result.standard_output;
    }

    emit GotGroovyScriptOutput(success, result);

    //Call the SIGNAL to teardown the JenkinsRequest
    emit Finished();
}


Notification::Severity JenkinsRequest::getJobConsoleOutput(QString job_name, int build_number, QString configuration){
    Notification::Severity job_state = Notification::Severity::NONE;

    if(BlockUntilValidatedSettings()){
        //Get the cached url
        QString console_url = getURL() + "job/" + combineJobURL(job_name, build_number, configuration) + "/consoleText";

        //Get an authenticated request
        auto console_request = getAuthenticatedRequest(console_url);

        //Use these to store info
        
        QString console_output;

        //While the job is either building or not yet existant.
        while(true){
            //Get the new_state of the job
            auto new_state = _getJobState(job_name, build_number, configuration);
            if(new_state != job_state){
                //If it's changed send a signal
                job_state = new_state;
                emit GotJobStateChange(job_name, build_number, configuration, job_state);
            }

            //If we are building get the console_data
            auto console_result = GetRunner()->HTTPGet(console_request);

            if(console_result.success){
                auto new_data = console_result.standard_output;
                if(new_data.size() > console_output.size()){
                    //Emit the live difference
                    QString delta_data = new_data.mid(console_output.size());
                    emit GotLiveJobConsoleOutput(job_name, build_number, configuration, delta_data.trimmed());
                    console_output = new_data;
                }
            }

            //Terminate out of while loop
            if(job_state != Notification::Severity::NONE && job_state != Notification::Severity::RUNNING){
                break;
            }
            QThread::msleep(TIME_OUT_MS);
        }

        emit GotJobConsoleOutput(job_name, build_number, configuration, console_output);
    }
    return job_state;
}

void JenkinsRequest::GetJobConsoleOutput(QString job_name, int build_number, QString configuration)
{
    getJobConsoleOutput(job_name, build_number, configuration);
    //Call the SIGNAL to teardown the JenkinsRequest
    emit Finished();
}

void JenkinsRequest::BuildJob(QString job_name, Jenkins_JobParameters parameters)
{
    int build_number = -1;
    Notification::Severity job_state = Notification::Severity::ERROR;
    
    NotificationObject* notification = 0;
    if(BlockUntilValidatedSettings()){
        notification = NotificationManager::manager()->AddNotification("Waiting for Jenkins to handle build request '" + job_name + "'", "Icons", "jenkinsFlat", Notification::Severity::RUNNING, Notification::Type::MODEL, Notification::Category::JENKINS);

        QUrlQuery query;
        //Add Parameters
        foreach(auto parameter, parameters){
            query.addQueryItem(parameter.name, QUrl::toPercentEncoding(parameter.value));
        }

        auto build_url = getURL() + "job/" + job_name + "/buildWithParameters";
        auto build_request = getAuthenticatedRequest(build_url);

        //Post parameters to buildWithParameters API
        auto build_result = GetRunner()->HTTPPost(build_request, query.toString().toUtf8());

        if(build_result.success){
            //Get the url of the stage job
            auto stage_url = build_result.location_header + "api/json";
            auto stage_request = getAuthenticatedRequest(stage_url);

            //We need to wait until the job has been given an executor
            while(true){
                auto stage_result = GetRunner()->HTTPGet(stage_request);

                if(stage_result.success){
                    //Parse the Json
                    auto json = QJsonDocument::fromJson(stage_result.standard_output.toUtf8()).object();
                    auto json_build_number = json["executable"].toObject()["number"].toInt(-1);
                    if(json_build_number != -1){
                        build_number = json_build_number;
                        break;
                    }
                }
                QThread::msleep(TIME_OUT_MS);
            }
        }

        if (build_number > 0) {
            notification->setDescription("Started Jenkins job: '" + job_name + "' #" + QString::number(build_number));
            
            job_state = getJobConsoleOutput(job_name, build_number, "");
        }
    }

    if(notification){
        if (build_number == -1) {
            notification->setDescription("Failed to request Jenkins build '" + job_name + "'");
            notification->setSeverity(Notification::Severity::ERROR);
        }else{
            auto severity = job_state;
            auto description = "Jenkins job: '" + job_name + "' #" + QString::number(build_number);
            auto icon_name = "sphereBlue";
            switch(severity){
                case Notification::Severity::ERROR:
                    description += " failed";
                    icon_name = "sphereRed";
                    break;
                case Notification::Severity::INFO:
                    description += " was aborted";
                    icon_name = "sphereGray";
                    break;
                default:
                    severity = Notification::Severity::SUCCESS;
                    description += " finished";
                    break;
            }
            //Use Jenkins Icons!
            notification->setIcon("Icons", icon_name);
            notification->setSeverity(severity);
            notification->setDescription(description);
        }
    }
    emit Finished();
}

void JenkinsRequest::StopJob(QString job_name, int build_number, QString configuration)
{
    if(BlockUntilValidatedSettings()){
        auto stop_url = getURL() + "job/" + combineJobURL(job_name, build_number, configuration) + "/stop";
        auto stop_request = getAuthenticatedRequest(stop_url);

        auto notification = NotificationManager::manager()->AddNotification("Requesting to stop Jenkins job '" + job_name + "' #" + QString::number(build_number), "Icons", "jenkinsFlat", Notification::Severity::RUNNING, Notification::Type::MODEL, Notification::Category::JENKINS);
        auto stop_result = GetRunner()->HTTPPost(stop_request);

        notification->setSeverity(stop_result.success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
        if(stop_result.success){
            notification->setDescription("Successfully requested to stop Jenkins job '" + job_name + "' #" + QString::number(build_number));
        }else{
            notification->setDescription("Failed to stop Jenkins job '" + job_name + "' #" + QString::number(build_number));
        }
    }

    emit Finished();
}

void JenkinsRequest::Teardown_()
{
    QMutexLocker(&this->mutex_);
    terminate_ = true;
    jenkins_manager_ = 0;
}

QString JenkinsRequest::getURL()
{
    QString url;
    QMutexLocker(&this->mutex_);

    if(jenkins_manager_){
        url = jenkins_manager_->GetUrl();
    }
    return url;
}

QString JenkinsRequest::getJobName()
{
    QString job_name;
    QMutexLocker(&this->mutex_);

    if(jenkins_manager_){
        job_name = jenkins_manager_->GetJobName();
    }
    return job_name;
}

bool JenkinsRequest::gotValidatedSettings()
{
    bool valid = false;
    QMutexLocker(&this->mutex_);

    if(jenkins_manager_){
        valid = jenkins_manager_->GotValidSettings();
    }
    return valid;
}

QNetworkRequest JenkinsRequest::getAuthenticatedRequest(QString url, bool auth)
{
    QNetworkRequest request;
    QMutexLocker(&this->mutex_);

    if(jenkins_manager_){
        request = jenkins_manager_->getAuthenticatedRequest(url, auth);
    }
    return request;
}

ProcessRunner *JenkinsRequest::GetRunner()
{
    if(!runner_){
        //We can't construct a ProcessRunner in the Constructor as it would be owned by the wrong thread
        runner_ = new ProcessRunner(this);
        connect(this, &JenkinsRequest::Terminate_, runner_, &ProcessRunner::Cancel);
    }
    return runner_;
}

QString JenkinsRequest::combineJobURL(QString job_name, int build_number, QString configuration)
{
    QString result = job_name;

    if(!configuration.isEmpty()){
        result += "/" + configuration;
    }
    if(build_number != 0){
        result += "/" + QString::number(build_number);
    }
    return result;
}

void JenkinsRequest::ValidateSettings()
{
    auto api_url = getURL() + "api/json";
    auto api_request = getAuthenticatedRequest(api_url, false);

    //Run an un-authed api call first
    auto api_result = GetRunner()->HTTPGet(api_request);

    //If we succeeded, re-run an authorized command
    if(api_result.success){
        auto api_auth_request = getAuthenticatedRequest(api_url, true);
        api_result = GetRunner()->HTTPGet(api_auth_request);
    }

    QString result;

    if(!api_result.success){
        QNetworkReply::NetworkError error = (QNetworkReply::NetworkError) api_result.error_code;
        switch(error){
            case QNetworkReply::HostNotFoundError:{
                result = "Host Not Found";
                break;
            }
            case QNetworkReply::ContentNotFoundError:{
                result = "Isn't a Jenkins Server";
                break;
            }
            case QNetworkReply::ProtocolUnknownError:{
                result = "Protocol Error";
                break;
            }
            case QNetworkReply::AuthenticationRequiredError:{
                result = "API User/Token Authentication Failed";
                break;
            }
            default:{
                result = "Unknown Error";
                break;
            }
        }
    }else{
        //Check for the job
        auto json = QJsonDocument::fromJson(api_result.standard_output.toUtf8()).object();

        //Get the name of the job from the manager
        QString job_name = getJobName();

        bool got_match = false;

        //Append The name of the Active Configurations to the configurationList
        foreach(QJsonValue j, json["jobs"].toArray()){
            auto job = j.toObject()["name"].toString();
            if(job == job_name){
                got_match = true;
                break;
            }
        }

        if(!got_match){
            result = "No Job Called '" + job_name + "'";
        }
    }

    emit GotValidatedSettings(result.isEmpty(), result);
    emit Finished();
}

Notification::Severity JenkinsRequest::_getJobState(QString jobName, int buildNumber, QString activeConfiguration)
{
    //Get the JSON data about this job.
    QJsonDocument configuration  = _getJobConfiguration(jobName, buildNumber, activeConfiguration, true);

    //If the jobJSON isn't null, check for activeConfigurations.
    if(!configuration.isNull()){
        QJsonObject configData = configuration.object();

        //Get the array which contains the result.
        bool building = configData["building"].toBool();
        if(building){
            return Notification::Severity::RUNNING;
        }else{
            QString result = configData["result"].toString();
            if(result == "SUCCESS"){
                return Notification::Severity::SUCCESS;
            }else if(result == "ABORTED"){
                return Notification::Severity::INFO;
            }else{
                return Notification::Severity::ERROR;
            }
        }
    }
    return Notification::Severity::NONE;
}
