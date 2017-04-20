#include "jenkinsrequest.h"
#include "jenkinsmanager.h"
#include <QNetworkReply>
#include <QTimer>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#include <QRegularExpression>
#include <QAuthenticator>
#include <QUrlQuery>
#include "../../Controllers/NotificationManager/notificationmanager.h"

#define TIME_OUT_MS 100

/**
 * @brief JenkinsRequest::JenkinsRequest Constructor for a JenkinsRequest Thread. A JenkinsRequest Thread is used for a single request from the Jenkins Server. Connects all request SIGNALS to there matching private SLOTS.
 * @param m The JenkinsManager which spawned this JenkinsRequest.
 */
JenkinsRequest::JenkinsRequest(JenkinsManager *m)
{
    managers = 0;
    manager = m;
    terminated = false;
    //networkManager = 0;
    timeOutMS = TIME_OUT_MS;

    //Connect the unexpected Termination signal to a slot.
    connect(this, SIGNAL(unexpectedTermination()), this, SLOT(_unexpectedTermination()));
    connect(this, SIGNAL(requestFinished()), this, SLOT(deleteLater()));

    if(!manager){
        qCritical() << "Cannot Construct JenkinsRequest, JenkinsManager provided is NULL";
    }
}

/**
 * @brief JenkinsRequest::~JenkinsRequest Destructor, will remove this JenkinsRequest from the JenkinsManager's Vector of JenkinsRequest Objects.
 */
JenkinsRequest::~JenkinsRequest()
{
    //qCritical() << "Removing this Jenkins Request";
    //Tell the manager this Request is done.
    if(manager){
        manager->jenkinsRequestFinished(this);
    }
    if(managers){
        managers->deleteLater();
    }
}


HTTPResult JenkinsRequest::wget2(QNetworkRequest request)
{
    HTTPResult result;

    QNetworkAccessManager* networkManager = getNetworkManager();

    if(networkManager){
        //Post to the URL from the networkManager.
        QNetworkReply* reply =  networkManager->get(request);
        result = waitForNetworkReply(reply);
        //Free up the memory of the Network Reply
        reply->deleteLater();
    }
    return result;
}

HTTPResult JenkinsRequest::post2(QNetworkRequest request, QByteArray post_data)
{
    HTTPResult result;

    QNetworkAccessManager* networkManager = getNetworkManager();

    if(networkManager){
        //Post to the URL from the networkManager.
        QNetworkReply* reply =  networkManager->post(request, post_data);
        result = waitForNetworkReply(reply);
        //Free up the memory of the Network Reply
        reply->deleteLater();
    }
    return result;
}
HTTPResult JenkinsRequest::waitForNetworkReply(QNetworkReply *reply)
{
    HTTPResult result;

    while(true){
        if(reply->isRunning()){
            //Construct a wait loop
            QEventLoop waitLoop;

            //Connect to Standard output/error signals
            connect(reply, &QNetworkReply::readyRead, &waitLoop, &QEventLoop::quit);
            connect(reply, &QNetworkReply::finished, &waitLoop, &QEventLoop::quit);
            //Block until a signal notifies
            waitLoop.exec();
        }

        qint64 bytes_available = reply->bytesAvailable();

        if(bytes_available > 0){
            //Attach Data.
            result.standard_output.append(reply->read(bytes_available));
        }

        if(bytes_available == 0 && reply->isFinished()){
            break;
        }
    }

    result.error_code = reply->error();
    result.success = reply->error() == QNetworkReply::NoError;
    result.location_header = reply->header(QNetworkRequest::LocationHeader).toString();
    return result;
}

/**
 * @brief JenkinsRequest::waitForValidSettings Waits for the JenkinsManager to have validated Settings. Will time out after timeOutMS.
 * @return true/false for valid settings.
 */
bool JenkinsRequest::waitForValidSettings()
{
    if(manager){
        if(manager->hasValidatedSettings()){
            return true;
        }
    }

    QTimer* timeOutTimer = new QTimer();
    timeOutTimer->setSingleShot(true);

    QEventLoop waitLoop;
    connect(timeOutTimer, SIGNAL(timeout()), &waitLoop, SLOT(quit()));
    connect(manager, SIGNAL(settingsValidationComplete(bool, QString)), &waitLoop, SLOT(quit()));
    connect(this, SIGNAL(unexpectedTermination()), &waitLoop, SLOT(quit()));

    //Start the timer
    timeOutTimer->start(4 * timeOutMS);

    //Wait for something to quit the EventLoop
    waitLoop.exec();
    bool timedOut = !timeOutTimer->isActive();

    //Stop the timer
    timeOutTimer->stop();

    //Delete the timer
    timeOutTimer->deleteLater();


    //Check for termination
    if(terminated || timedOut){
        qCritical() << "Terminating the Jenkins Request!";
        return false;
    }else{
        //
        //Return what the value of the validation returned.
        return manager->hasValidatedSettings();
    }
}


/**
 * @brief JenkinsRequest::getNetworkManager Gets a new QNetworkAccessManager to use, Connects the Authentication request into the JenkinsManager
 * @return The QNetworkAccess Manager
 */
QNetworkAccessManager *JenkinsRequest::getNetworkManager()
{
    //If we don't have a networkManager, we should construct one.
    if(!managers){
        managers = new QNetworkAccessManager();
    }
    return managers;
}

/**
 * @brief JenkinsRequest::getJobConfiguration Gets the JSON configuration of a Jenkins job. Will store this configuration in it's JenkinsManager if it requests new information.
 * @param jobName The name of the Jenkins job
 * @param buildNumber The build number of the job (Can be -1 for the root job)
 * @param activeConfiguration The name of the Active Configuration (Can be empty for the root job)
 * @param reRequest If set to true, this function will request fresh JSON data from the Jenkins Server.
 * @return The Configuration in JSON format for the provided job.
 */
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


    if(manager){
        //Get a cached copy
        configuration = manager->getJobConfiguration(cached_name);

        if(configuration.isNull() || refresh){
            QString config_url = manager->getURL() + "job/" + cached_name + "/api/json";
            auto config_request = manager->getAuthenticatedRequest(config_url);
            auto config_result = wget2(config_request);

            if(config_result.success){
                //Try and load the configuration
                configuration = QJsonDocument::fromJson(config_result.standard_output.toUtf8());
                if(!configuration.isNull()){
                    manager->storeJobConfiguration(cached_name, configuration);
                }
            }
        }
    }
    return configuration;
}


/**
 * @brief JenkinsRequest::getJobParameters Gets the List of Build Parameters for the Jenkins job, with name JobName. Emits a singular gotJobParameters signal after data has been retrieved from the Jenkins Server. Emits a requestFinished signal on completion.
 * @param jobName The name of the Jenkins job
 */
void JenkinsRequest::getJobParameters(QString jobName)
{
     if(waitForValidSettings()){
         Jenkins_JobParameters jobParameters;

        //Get the configuration for the Job.
        QJsonDocument configuration = _getJobConfiguration(jobName);

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
        emit gotJobParameters(jobName, jobParameters);

    }else{
        emit requestFailed();
    }
    //Emit the SIGNAL to state the JenkinsRequest
    emit requestFinished();
}

void JenkinsRequest::validateJenkinsSettings()
{
    QString vError = validate();
    emit gotSettingsValidationResponse(vError.isEmpty(), vError);
    emit requestFinished();
}


/**
 * @brief JenkinsRequest::runGroovyScript Executes a Groovy Script(File) on the Jenkins Server and returns the data
 * @param groovyScriptPath The Absolute path to the Groovy Script file.
 */
void JenkinsRequest::runGroovyScript(QString groovy_script)
{
    bool success = false;
    QString result;

    if(waitForValidSettings()){
        QUrlQuery script;
        //Encode the script
        script.addQueryItem("script", QUrl::toPercentEncoding(groovy_script));

        auto script_url = manager->getURL() + "scriptText";
        auto script_request = manager->getAuthenticatedRequest(script_url);
        auto script_result = post2(script_request, script.toString().toUtf8());

        success = script_result.success;
        result = script_result.standard_output;
    }

    emit gotGroovyScriptOutput(success, result);

    //Call the SIGNAL to teardown the JenkinsRequest
    emit requestFinished();
}

/**
 * @brief JenkinsRequest::getJobConsoleOutput - Requests the Console Output from a job.
 * @param jobName
 * @param buildNumber
 * @param activeConfiguration
 */
void JenkinsRequest::getJobConsoleOutput(QString job_name, int build_number, QString configuration)
{
    if(waitForValidSettings()){
        //Get the cached url
        QString console_url = manager->getURL() + "job/" + combineJobURL(job_name, build_number, configuration) + "/consoleText";

        //Get an authenticated request
        auto console_request = manager->getAuthenticatedRequest(console_url);

        //Use these to store info
        JOB_STATE job_state = NO_JOB;
        QString console_output;

        //While the job is either building or not yet existant.
        while(true){
            //Get the new_state of the job
            auto new_state = _getJobState(job_name, build_number, configuration);
            if(new_state != job_state){
                //If it's changed send a signal
                job_state = new_state;
                emit gotJobStateChange(job_name, build_number, configuration, job_state);
            }

            //If we are building get the console_data
            auto console_result = wget2(console_request);

            if(console_result.success){
                auto new_data = console_result.standard_output;
                if(new_data.size() > console_output.size()){
                    //Emit the live difference
                    QString delta_data = new_data.mid(console_output.size());
                    emit gotLiveJobConsoleOutput(job_name, build_number, configuration, delta_data);
                    console_output = new_data;
                }
            }

            //Terminate out of while loop
            if(job_state != NO_JOB && job_state != BUILDING){
                break;
            }
            //Sleep
            QThread::msleep(timeOutMS);
        }

        emit gotJobConsoleOutput(job_name, build_number, configuration, console_output);
    }else{
        emit requestFailed();
    }
    //Call the SIGNAL to teardown the JenkinsRequest
    emit requestFinished();
}


/**
 * @brief JenkinsRequest::buildJob Starts and waits for a Job to build on the Jenkins Server. Calls getJobState to emit the state changes of the Job.
 * @param jobName The name of the Jenkins job
 * @param jobParameters A list of parameters for the Jenkis Job
 */
void JenkinsRequest::buildJob(QString job_name, Jenkins_JobParameters parameters)
{
    int build_number = -1;

    if(waitForValidSettings()){
        emit NotificationManager::manager()->backgroundProcess(true, BP_RUNNING_JOB);
        QUrlQuery query;

        //Add Parameters
        foreach(auto parameter, parameters){
            query.addQueryItem(parameter.name, QUrl::toPercentEncoding(parameter.value));
        }

        if(manager){
            auto build_url = manager->getURL() + "job/" + job_name + "/buildWithParameters";
            auto build_request = manager->getAuthenticatedRequest(build_url);

            //Post parameters to buildWithParameters API
            auto build_result = post2(build_request, query.toString().toUtf8());

            if(build_result.success){
                //Get the url of the stage job
                auto stage_url = build_result.location_header + "api/json";
                auto stage_request = manager->getAuthenticatedRequest(stage_url);

                //We need to wait until the job has been given an executor
                while(true){
                    auto stage_result = wget2(stage_request);

                    if(stage_result.success){
                        //Parse the Json
                        auto json = QJsonDocument::fromJson(stage_result.standard_output.toUtf8()).object();
                        auto json_build_number = json["executable"].toObject()["number"].toInt(-1);
                        if(json_build_number != -1){
                            build_number = json_build_number;
                            break;
                        }
                    }
                    QThread::msleep(timeOutMS);
                }
            }

            if(build_number > 0){
                NotificationManager::manager()->displayNotification("Started Jenkins Job: " + job_name + " #" + QString::number(build_number) , "Icons", "jenkins", -1, NS_INFO, NT_MODEL, NC_JENKINS);
                //Get the console output and state!
                getJobConsoleOutput(job_name, build_number);
            }
        }
    }
    emit NotificationManager::manager()->backgroundProcess(false, BP_RUNNING_JOB);
    //Emit failures if the build job failed to get a build number!
    if(build_number == -1){
        emit requestFailed();
        emit requestFinished();
    }else{
        NotificationManager::manager()->displayNotification("Jenkins Job: " + job_name + " #" + QString::number(build_number) + " Finished!" , "Icons", "jenkins", -1, NS_INFO, NT_MODEL, NC_JENKINS);
    }
}

/**
 * @brief JenkinsRequest::stopJob Sends a request to (and waits) abort a Job on the Jenkins Server.
 * @param jobName The name of the Jenkins job
 * @param buildNumber The build number of the job
 * @param activeConfiguration The name of the Active Configuration (Can be empty for the root job)
 */
void JenkinsRequest::stopJob(QString job_name, int build_number, QString configuration)
{
    bool success = false;
    if(waitForValidSettings()){

        auto stop_url = manager->getURL() + "job/" + combineJobURL(job_name, build_number, configuration) + "/stop";
        auto stop_request = manager->getAuthenticatedRequest(stop_url);

        NotificationManager::manager()->displayNotification("Stopping Jenkins Job: " + job_name + " #" + QString::number(build_number) , "Icons", "jenkins", -1, NS_INFO, NT_MODEL, NC_JENKINS);
        auto stop_result = post2(stop_request);
        success = stop_result.success;
    }

    if(!success){
        emit requestFailed();
    }
    emit requestFinished();
}

/**
 * @brief JenkinsRequest::_unexpectedTermination Called when this JenkinsRequest JenkinsManager is destroyed.
 * Sets the state of a terminating variable. and unsets the JenkinsManag.er
 */
void JenkinsRequest::_unexpectedTermination()
{
    terminated = true;
    //Try unset JenkinsManger
    manager = 0;
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

QString JenkinsRequest::validate()
{
    auto api_url = manager->getURL() + "api/json";
    auto api_request = manager->getAuthenticatedRequest(api_url, false);
    auto api_auth_request = manager->getAuthenticatedRequest(api_url, true);

    //Run an un-authed api call first
    auto api_result = wget2(api_request);

    //If we succeeded, re-run an authorized command
    if(api_result.success){
        api_result = wget2(api_auth_request);
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
        QString job_name = manager->getJobName();

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
    return result;
}



/**
 * @brief JenkinsRequest::_getJobState Gets the current state of a Jenkins job.
 * @param jobName The name of the Jenkins job
 * @param buildNumber The build number of the job (Can be -1 for the root job)
 * @param activeConfiguration The name of the Active Configuration (Can be empty for the root job)
 * @return The state of the Jenkins job.
 */
JOB_STATE JenkinsRequest::_getJobState(QString jobName, int buildNumber, QString activeConfiguration)
{
    //Get the JSON data about this job.
    QJsonDocument configuration  = _getJobConfiguration(jobName, buildNumber, activeConfiguration, true);

    //If the jobJSON isn't null, check for activeConfigurations.
    if(!configuration.isNull()){
        QJsonObject configData = configuration.object();

        //Get the array which contains the result.
        bool building = configData["building"].toBool();
        if(building){
            return BUILDING;
        }else{
            QString result = configData["result"].toString();
            if(result == "SUCCESS"){
                return BUILT;
            }else if(result == "ABORTED"){
                return ABORTED;
            }else{
                return FAILED;
            }
        }
    }
    return NO_JOB;
}
