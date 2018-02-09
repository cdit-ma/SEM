#include "jenkinsmanager.h"
#include "jenkinsrequest.h"

#include "../ViewController/viewcontroller.h"
#include "../ActionController/actioncontroller.h"
#include "../NotificationManager/notificationmanager.h"

#include "../../Utils/filehandler.h"
#include "../../Widgets/Dialogs/variabledialog.h"
#include "../../Widgets/Monitors/jenkinsmonitor.h"
#include "../../Widgets/Monitors/jobmonitor.h"

#include <QStringBuilder>
#include <QApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QFutureWatcher>
#include <QUrlQuery>
#include <QHttpMultiPart>


JenkinsManager::JenkinsManager(ViewController* view_controller) : QObject(view_controller)
{
    //Register the Types used as parameters JenkinsRequest so signals/slots can be connected.
    qRegisterMetaType<Jenkins_JobParameters>("Jenkins_JobParameters");
    qRegisterMetaType<Jenkins_Job_Statuses>("Jenkins_Job_Statuses");
    

    view_controller_ = view_controller;
    //Set script directory
    scipts_path_ = QApplication::applicationDirPath() % "/Resources/scripts/";

    //Connect to settings changes
    SettingsController* settings = SettingsController::settings();
    connect(settings, &SettingsController::settingChanged, this, &JenkinsManager::SettingChanged);
    connect(settings, &SettingsController::settingsApplied, this, &JenkinsManager::settingsApplied);

    //Sync settings to JenkinsManager
    foreach(SETTINGS key, settings->getSettingsKeys("Jenkins")){
        SettingChanged(key, settings->getSetting(key));
    }


    connect(this, &JenkinsManager::GotJenkinsNodes, view_controller, &ViewController::jenkinsManager_GotJenkinsNodesList);
    connect(this, &JenkinsManager::JenkinsReady, view_controller, &ViewController::GotJenkins);

    //connect(view_controller, &ViewController::vc_executeJenkinsJob, this, &JenkinsManager::BuildJob);

    auto action_controller = view_controller->getActionController();
    

    //Validate
    ValidateSettings();
}

QString JenkinsManager::GetUser()
{
    QMutexLocker lock(&var_mutex_);
    return username_;
}

void JenkinsManager::SetUrl(QString url)
{
    //Enforce a trailing /
    if(!url.endsWith("/")){
        url += "/";
    }

    QMutexLocker lock(&var_mutex_);
    if(url_ != url){
        url_ = url;
        settings_changed = true;
        settings_validated_ = false;
    }
}

void JenkinsManager::SetUser(QString user)
{
    QMutexLocker lock(&var_mutex_);
    if(username_ != user){
        username_ = user;
        settings_changed = true;
        settings_validated_ = false;
    }
}

void JenkinsManager::SetApiToken(QString api_token)
{
    QMutexLocker lock(&var_mutex_);
    if(token_ != api_token){
        token_ = api_token;
        settings_changed = true;
        settings_validated_ = false;
    }
}

void JenkinsManager::SetJobName(QString job_name)
{
    QMutexLocker lock(&var_mutex_);
    if(job_name_ != job_name){
        job_name_ = job_name;
        settings_changed = true;
        settings_validated_ = false;
    }
}

bool JenkinsManager::GotValidSettings()
{
    QMutexLocker lock(&var_mutex_);
    return settings_validated_;
}

bool JenkinsManager::WaitForSettingsValidation(){
    auto validate_future = ValidateSettings().second;
    validate_future.waitForFinished();
    return GotValidSettings();
}


void JenkinsManager::settingsApplied(){
    if(settings_changed){
        ValidateSettings();
    }
}


JenkinsRequest *JenkinsManager::GetJenkinsRequest(QObject *parent)
{
    //Always start validation, if necessary
    ValidateSettings();

    //Construct a new JenkinsRequest, with the JenkinsManager passed as a parameter
    //The request wil be placed onto another thread
    JenkinsRequest* request = new JenkinsRequest(this, parent);

    QMutexLocker lock(&var_mutex_);
    requests.append(request);
    return request;
}

QPair<bool, QFuture<QJsonDocument> > JenkinsManager::GetNodes()
{
    auto future_key = GetUrl() + "runscript/Get_Nodes";

    QMutexLocker lock(&futures_mutex_);
    
    //Get the future used by the api_url (If we don't have one it will construct a finished QFuture)
    auto &future = url_futures_[future_key];
    bool made_request = false;

    if(!future.isRunning()){
        auto script = FileHandler::readTextFile(scipts_path_ + "Jenkins_Construct_GraphMLNodesList.groovy");
        future = QtConcurrent::run(this, &JenkinsManager::RequestGroovyScriptExecution, QString("Request Jenkins Nodes"), script);
        made_request = true;
    }

    return {made_request, future};
}

SETTING_TYPE JenkinsManager::GetSettingType(QString &parameter_type){
    SETTING_TYPE type = SETTING_TYPE::STRING;
    
    if(parameter_type == "String"){
        type = SETTING_TYPE::STRING;
    }else if(parameter_type == "Boolean"){
        type = SETTING_TYPE::BOOL;  
    }else if(parameter_type == "File"){
        type = SETTING_TYPE::FILE;
    }
    return type;
}

void JenkinsManager::BuildJob(QString model_file)
{
    if(GotValidSettings()){
        //Construct a new JenkinsRequest Object.
        auto jenkins_request = GetJenkinsRequest(this);
    
        //Connect the emit signals from this Thread to the JenkinsRequest Thread.
        auto r = connect(this, &JenkinsManager::getJobParameters, jenkins_request, &JenkinsRequest::GetJobParameters);
        //Run some lambda
        connect(jenkins_request, &JenkinsRequest::GotJobParameters, this, [=](QString job_name, Jenkins_JobParameters parameters){

            VariableDialog dialog("Jenkins: " + job_name_ + " Parameters");

            for(auto parameter : parameters){
                auto default_value = parameter.defaultValue;
                bool is_file_model = parameter.name == "model" && (parameter.type == SETTING_TYPE::FILE);
                
                if(is_file_model){
                    default_value = model_file;
                }

                dialog.addOption(parameter.name, parameter.type, default_value);
                dialog.setOptionIcon(parameter.name, "Icons", "label");
                //Disable model upload
                dialog.setOptionEnabled(parameter.name, !is_file_model);
            }

            auto options = dialog.getOptions();

            auto got_options = options.size() == parameters.size();
            
            if(got_options){
                for(auto& parameter : parameters){
                    parameter.value = dialog.getOptionValue(parameter.name).toString();
                }

                auto jenkins_build = GetJenkinsRequest();
            
                auto build_job = connect(this, &JenkinsManager::buildJob, jenkins_build, &JenkinsRequest::BuildJob);
                
                emit buildJob(job_name_, parameters);
                disconnect(build_job);

                connect(jenkins_build, &JenkinsRequest::GotJobStateChange, this, &JenkinsManager::gotJobStateChange);
                connect(jenkins_build, &JenkinsRequest::GotLiveJobConsoleOutput, this, &JenkinsManager::gotJobConsoleOutput);
                connect(jenkins_build, &JenkinsRequest::BuildingJob, this, &JenkinsManager::BuildingJob);
                
            }
        }, Qt::QueuedConnection);
        
        emit getJobParameters(job_name_);
        disconnect(r);
    }
}

void JenkinsManager::GetJobConsoleOutput(QString job_name, int job_number){
    auto jenkins_request = GetJenkinsRequest();
            
    auto request_console_job = connect(this, &JenkinsManager::getJobConsole, jenkins_request, &JenkinsRequest::GetJobConsoleOutput);
    emit getJobConsole(job_name, job_number);
    disconnect(request_console_job);

    connect(jenkins_request, &JenkinsRequest::GotJobStateChange, this, &JenkinsManager::gotJobStateChange);
    connect(jenkins_request, &JenkinsRequest::GotLiveJobConsoleOutput, this, &JenkinsManager::gotJobConsoleOutput);
    connect(jenkins_request, &JenkinsRequest::GotJobArtifacts, this, &JenkinsManager::gotJobArtifacts);
}

void JenkinsManager::GetRecentJobs(QString job_name){
    auto jenkins_request = GetJenkinsRequest();
            
    auto request_job = connect(this, &JenkinsManager::getRecentJobs, jenkins_request, &JenkinsRequest::GetRecentJobs);
    auto filter_by_active_user = SettingsController::settings()->getSetting(SETTINGS::JENKINS_REQUEST_USER_JOBS).toBool();


    emit getRecentJobs(job_name, 10, filter_by_active_user);
    disconnect(request_job);

    connect(jenkins_request, &JenkinsRequest::gotRecentJobs, this, &JenkinsManager::gotRecentJobs);
}




void JenkinsManager::GotoJob(QString job_name, int build_number){
    auto url = GetUrl() + "job/" + job_name + "/" + QString::number(build_number);
    view_controller_->openURL(url);
}


void JenkinsManager::AbortJob2(QString job_name, int job_number){
}



void JenkinsManager::SettingChanged(SETTINGS key, QVariant value)
{
    QString strValue = value.toString();

    switch(key){
    case SETTINGS::JENKINS_API:{
        SetApiToken(strValue);
        break;
    }
    case SETTINGS::JENKINS_JOBNAME:{
        SetJobName(strValue);
        break;
    }
    case SETTINGS::JENKINS_USER:{
        SetUser(strValue);
        break;
    }
    case SETTINGS::JENKINS_URL:{
        SetUrl(strValue);
        break;
    }
    case SETTINGS::JENKINS_APPLY:{
        ValidateSettings();
        break;
    }
    default:
        break;
    }
}


QPair<bool, QFuture<QJsonDocument> > JenkinsManager::ValidateSettings()
{
    auto api_url = GetUrl() + "/api/json";
    QMutexLocker lock(&futures_mutex_);
    
    //Get the future used by the api_url (If we don't have one it will construct a finished QFuture)
    auto &future = url_futures_[api_url];
    bool made_request = false;
    if(!GotValidSettings() && !future.isRunning()){
        future = QtConcurrent::run(this, &JenkinsManager::RequestValidation);
        made_request = true;
    }

    return {made_request, future};
}

QPair<bool, QFuture<QJsonDocument> > JenkinsManager::GetJobConfiguration(QString job_name, int job_number, bool re_request)
{
    qCritical() << "JenkinsManager::GetJobConfiguration()" << job_name << ":" << job_number;

    auto api_url = GetUrl() + GetJobStatusKey(job_name, job_number) + "/api/json";
    QMutexLocker lock(&futures_mutex_);
    
    //Get the future used by the api_url (If we don't have one it will construct a finished QFuture)
    auto got_result = url_futures_.contains(api_url);
    auto &future = url_futures_[api_url];
    bool made_request = false;

    //Only request if we aren't already running one, and we actually need to
    if(!future.isRunning() && (!got_result || re_request)){
        
        future = QtConcurrent::run(this, &JenkinsManager::RequestJobConfiguration, job_name, job_number);
        made_request = true;
    }

    return {made_request, future};
}

QJsonDocument JenkinsManager::RequestJobConfiguration(QString job_name, int job_number)
{
    qCritical() << "JenkinsManager::RequestJobConfiguration()" << job_name << ":" << job_number;
    QJsonDocument document;
    
    if(WaitForSettingsValidation()){
        ProcessRunner runner;
        auto api_url = GetUrl() + "job/" + GetJobStatusKey(job_name, job_number) + "/api/json";
        auto api_request = getAuthenticatedRequest(api_url);
        auto api_result = runner.HTTPGet(api_request);

        qCritical() << api_url;
        
        if(api_result.success){
            document = QJsonDocument::fromJson(api_result.standard_output.toUtf8());
        }
    }
    return document;
}

QPair<bool, QFuture<Jenkins_JobParameters> > JenkinsManager::GetJobParameters(QString job_name){
    qCritical() << "JenkinsManager::GetJobParameters()" << job_name;

    auto api_url = GetUrl() + GetJobStatusKey(job_name) + "/get_job_parameters";
    QMutexLocker lock(&futures_mutex_);
    
    //Get the future used by the api_url (If we don't have one it will construct a finished QFuture)
    auto got_result = parameter_futures_.contains(api_url);
    auto &future = parameter_futures_[api_url];
    bool made_request = false;

    //Only request if we aren't already running one, and we actually need to
    if(!future.isRunning() && !got_result){
        future = QtConcurrent::run(this, &JenkinsManager::RequestJobParameters, job_name);
        made_request = true;
    }

    return {made_request, future};
}


Jenkins_JobParameters JenkinsManager::RequestJobParameters(QString job_name){
    qCritical() << "RequestJobParameters" << job_name;
    Jenkins_JobParameters job_parameters;

    //Request the job configuration
    auto json_future = GetJobConfiguration(job_name, -1, false).second;
    qCritical() << "Waiting On Results";
    auto json = json_future.result();
    
    if(!json.isNull()){
        auto config = json.object();
        
        auto job_type = config["_class"].toString();

        QJsonArray parameter_list;

        if(job_type == "hudson.matrix.MatrixProject"){
            for(auto action: config["actions"].toArray()){
                auto action_obj = action.toObject();
                
                if(action_obj["_class"].toString() == "hudson.model.ParametersDefinitionProperty"){
                    parameter_list = action_obj["parameterDefinitions"].toArray();
                    break;
                }
            }
        }else if(job_type == "org.jenkinsci.plugins.workflow.job.WorkflowJob"){
            for(auto property: config["property"].toArray()){
                auto property_obj = property.toObject();
                if(property_obj["_class"].toString() == "hudson.model.ParametersDefinitionProperty"){
                    parameter_list = property_obj["parameterDefinitions"].toArray();
                    break;
                }
            }
        }else{
            qCritical() << "JenkinsManager::RequestJobParameters(" << job_name << "): Can't handle Job of Type: '" << job_type << "'";
        }

        for(auto parameter: parameter_list){
            auto parameter_obj = parameter.toObject();

            Jenkins_Job_Parameter job_parameter;
            job_parameter.name = parameter_obj["name"].toString();
            job_parameter.description = parameter_obj["description"].toString();

            //The type is in format [type]ParameterDefinition, so trim this.
            auto type_str = parameter_obj["type"].toString().replace("ParameterDefinition", "");
            job_parameter.type = GetSettingType(type_str);
            
            auto default_value = parameter_obj["defaultParameterValue"].toObject()["value"];
            if(default_value.isBool()){
                if(default_value.toBool()){
                    job_parameter.defaultValue = "true";
                }else{
                    job_parameter.defaultValue = "false";
                }
            }else{
                job_parameter.defaultValue = default_value.toString();
            }
            //Add it the jobParameter List
            job_parameters.append(job_parameter);
        }
    }
    return job_parameters;
}

QPair<bool, QFuture<QJsonDocument> > JenkinsManager::BuildJob(QString job_name, Jenkins_JobParameters job_parameters){
    qCritical() << "JenkinsManager::BuildJob()" << job_name;
    auto api_url = GetUrl() + "/" + job_name + "/build";
    QMutexLocker lock(&futures_mutex_);
    
    //Get the future used by the api_url (If we don't have one it will construct a finished QFuture)
    auto &future = url_futures_[api_url];
    bool made_request = false;
    if(!future.isRunning()){
        future = QtConcurrent::run(this, &JenkinsManager::RequestBuildJob, job_name, job_parameters);
        made_request = true;
    }
    return {made_request, future};
}

QJsonDocument JenkinsManager::RequestBuildJob(QString job_name, Jenkins_JobParameters job_parameters)
{
    qCritical() << "JenkinsManager::RequestBuildJob()" << job_name;
    bool result = false;
    
    auto notification = NotificationManager::manager()->AddNotification("Build Job '" + job_name + "'", "Icons", "jenkinsFlat", Notification::Severity::RUNNING, Notification::Type::MODEL, Notification::Category::JENKINS);
    
    if(WaitForSettingsValidation()){
        auto multi_part = new QHttpMultiPart(QHttpMultiPart::FormDataType);
        {
            //Construct a Json Parameter Array
            QJsonArray parameter_array_json;

            for(auto &parameter: job_parameters){
                QJsonObject parameter_json;
                parameter_json.insert("name", QJsonValue::fromVariant(parameter.name));

                if(parameter.type == SETTING_TYPE::FILE){
                    parameter_json.insert("file", QJsonValue::fromVariant(parameter.name));
                }else{
                    parameter_json.insert("value", QJsonValue::fromVariant(parameter.value));
                }
                parameter_array_json.push_back(parameter_json);
            }

            QJsonObject parameter_object;
            parameter_object.insert("parameter", parameter_array_json);
            
            //Construct a string representation of the JSON
            auto parameter_json = QJsonDocument(parameter_object).toJson(QJsonDocument::Compact);
            
            QHttpPart json_part;
            json_part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
            json_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"json\""));
            json_part.setBody(parameter_json);
            multi_part->append(json_part);
        }

        {
            //Attach the file parameter's by reading the file
            for(auto &parameter: job_parameters){
                if(parameter.type == SETTING_TYPE::FILE){
                    auto file = new QFile(parameter.value);
                    //Open a file to read the file
                    if(file->open(QIODevice::ReadOnly)){
                        QFileInfo file_info(*file);

                        QHttpPart file_part;
                        file_part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
                        file_part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"" + parameter.name + "\"; filename=\""+ file_info.baseName() + "\""));
                        file_part.setBodyDevice(file);
                        file->setParent(multi_part);
                        multi_part->append(file_part);
                    }
                }
            }
        }

        ProcessRunner runner;
        auto build_url = GetUrl() + "job/" + job_name + "/build?delay=0sec";
        auto build_request = getAuthenticatedRequest(build_url);
        auto build_result = runner.HTTPPostMulti(build_request, multi_part);
        result = build_result.success;
    }

    if(notification){
        if (result) {
            notification->setSeverity(Notification::Severity::SUCCESS);
            notification->setTitle("Started Jenkins Job: '" + job_name + "'");
        }else{
            notification->setSeverity(Notification::Severity::ERROR);
            notification->setTitle("Failed to request Jenkins build '" + job_name + "'");
        }
    }

    QJsonObject json_result;
    json_result.insert("result", QJsonValue::fromVariant(result));
    return QJsonDocument(json_result);
}





QPair<bool, QFuture<QJsonDocument> > JenkinsManager::AbortJob(QString job_name, int job_number){
    auto api_url = GetUrl() + "job/" + job_name + "/" + QString::number(job_number) + "/stop";
    QMutexLocker lock(&futures_mutex_);
    
    //Get the future used by the api_url (If we don't have one it will construct a finished QFuture)
    auto &future = url_futures_[api_url];
    bool made_request = false;

    if(!GotValidSettings() && !future.isRunning()){
        future = QtConcurrent::run(this, &JenkinsManager::RequestAbortJob, job_name, job_number);
        made_request = true;
    }

    return {made_request, future};
}


QJsonDocument JenkinsManager::RequestAbortJob(QString job_name, int job_number){
    bool success = false;
    QJsonObject json_result;

    if(WaitForSettingsValidation()){
        auto job_number_str = QString::number(job_number);
        auto notification = NotificationManager::manager()->AddNotification("Stopping Jenkins Job '" + job_name + "' #" + job_number_str, "Icons", "jenkinsFlat", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::JENKINS);
        
        ProcessRunner runner;
        auto api_url = GetUrl() + "job/" + job_name + "/" + job_number_str + "/stop";
        auto api_request = getAuthenticatedRequest(api_url);
        auto api_result = runner.HTTPPost(api_request);
        success = api_result.success;

        notification->setTitle(api_result.success ? "Stopped Jenkins Job '" + job_name + "' #" + job_number_str : "Failed to stop Jenkins Job '" + job_name + "' #" + job_number_str);
        notification->setSeverity(api_result.success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    }
    json_result.insert("result", QJsonValue::fromVariant(success));
    return QJsonDocument(json_result);
}

void JenkinsManager::GetNodes_(QString groovy_script){
    if(GotValidSettings()){
        auto notification = NotificationManager::manager()->AddNotification("Requesting Jenkins Nodes", "Icons", "jenkinsFlat", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::JENKINS);
        
        ProcessRunner runner;
        
        QUrlQuery script;
        //Encode the script
        script.addQueryItem("script", QUrl::toPercentEncoding(groovy_script));
        
        auto script_bytes = script.toString().toUtf8();
        auto api_request = getAuthenticatedRequest(GetUrl() + "scriptText");
        auto api_result = runner.HTTPPost(api_request, script_bytes);
        auto graphml = api_result.standard_output;
    
        auto success = api_result.success && graphml.size();
        if(success){
            emit GotJenkinsNodes(graphml);
        }
        notification->setSeverity(success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
        notification->setTitle(success ? "Successfully requested Jenkins nodes" : "Failed to request Jenkins nodes: ");
    }
}

QJsonDocument JenkinsManager::RequestValidation(){

    //Construct a Notification for validating
    auto notification = NotificationManager::manager()->AddNotification("Validating Jenkins Settings", "Icons", "jenkinsFlat", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::JENKINS);
    auto api_url = GetUrl() + "/api/json";
    auto api_request = getAuthenticatedRequest(api_url, false);

    //Construct a process runner
    ProcessRunner runner;
    
    bool result = false;
    QString result_string;

    //Run an un-authed api call first (To Check for server existance)
    auto api_result = runner.HTTPGet(api_request);

    //If we succeeded, re-run an authorized command
    if(api_result.success){
        auto api_auth_request = getAuthenticatedRequest(api_url, true);
        api_result = runner.HTTPGet(api_auth_request);
    }

    if(api_result.success){
        //Get the JSON our request
        auto json = QJsonDocument::fromJson(api_result.standard_output.toUtf8()).object();
        auto required_job_name = GetJobName();

        auto got_job = false;

        //Check through the list of jobs to 
        for(auto j : json["jobs"].toArray()){
            auto job_name = j.toObject()["name"].toString();
            if(job_name == required_job_name){
                got_job = true;
                break;
            }
        }
        
        if(got_job){
            result = true;
        }else{
            result_string = "No Job Called '" + required_job_name + "'";
        }
    }else{
        auto error = (QNetworkReply::NetworkError) api_result.error_code;
        switch(error){
            case QNetworkReply::HostNotFoundError:{
                result_string = "Host Not Found";
                break;
            }
            case QNetworkReply::ContentNotFoundError:{
                result_string = "Isn't a Jenkins Server";
                break;
            }
            case QNetworkReply::ProtocolUnknownError:{
                result_string = "Protocol Error";
                break;
            }
            case QNetworkReply::AuthenticationRequiredError:{
                result_string = "API User/Token Authentication Failed";
                break;
            }
            default:{
                result_string = "Unknown Error";
                break;
            }
        }
    }

    //Update the notification
    notification->setSeverity(result ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    notification->setTitle(result ? "Jenkins settings validated successfully": "Jenkins settings invalid");
    notification->setDescription(result_string);

    SetSettingsValidated(result);

    QJsonObject json_result;
    json_result.insert("result", QJsonValue::fromVariant(result));

    return QJsonDocument(json_result);
}

QJsonDocument JenkinsManager::RequestGroovyScriptExecution(QString title, QString groovy_script){
    QJsonObject json_result;
    {
        auto validate_future = ValidateSettings().second;
        validate_future.waitForFinished();
    }

    if(GotValidSettings()){
        auto notification = NotificationManager::manager()->AddNotification(title, "Icons", "jenkinsFlat", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::JENKINS);
        
        ProcessRunner runner;
        
        QUrlQuery script;
        //Encode the script
        script.addQueryItem("script", QUrl::toPercentEncoding(groovy_script));
        
        auto script_bytes = script.toString().toUtf8();
        auto api_request = getAuthenticatedRequest(GetUrl() + "scriptText");
        auto api_result = runner.HTTPPost(api_request, script_bytes);
        auto success = api_result.success;

        json_result.insert("result", QJsonValue::fromVariant(success));
        json_result.insert("graphml", QJsonValue::fromVariant(api_result.standard_output));

        notification->setSeverity(success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
        //notification->setTitle(title);
    }
    return QJsonDocument(json_result);
}

void JenkinsManager::SetSettingsValidated(bool valid){
    qCritical() << "VALIDATED SETTINGS";
    QMutexLocker lock(&var_mutex_);
    settings_validated_ = valid;
    settings_changed = false;
    jobsJSON.clear();
    emit JenkinsReady(settings_validated_);
}


void JenkinsManager::jenkinsRequestFinished(JenkinsRequest *request)
{
    requests.removeAll(request);
}

QString JenkinsManager::GetUrl()
{
    QMutexLocker lock(&var_mutex_);
    return url_;
}

void JenkinsManager::storeJobConfiguration(QString jobName, QJsonDocument json)
{
    QMutexLocker lock(&var_mutex_);
    jobsJSON[jobName] = json;
}

QNetworkRequest JenkinsManager::getAuthenticatedRequest(QString url, bool auth)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    //Attach Headers
    request.setHeader(QNetworkRequest::UserAgentHeader, "JenkinsManager-Request");

    if(auth){
        // HTTP Basic authentication header value: base64(username:password)
        QString token = GetUser() + ":" + GetApiToken();
        QByteArray data = token.toLocal8Bit().toBase64();
        QByteArray auth_data = QString("Basic " + data).toLocal8Bit();
        request.setRawHeader("Authorization", auth_data);
    }
    return request;
}

QJsonDocument JenkinsManager::getJobConfiguration(QString job_name)
{    
    QMutexLocker lock(&var_mutex_);
    return jobsJSON.value(job_name, QJsonDocument());
}

QString JenkinsManager::GetJobName()
{
    QMutexLocker lock(&var_mutex_);
    return job_name_;
}

QString JenkinsManager::GetApiToken()
{
    QMutexLocker lock(&var_mutex_);
    return token_;
}

Jenkins_Job_Status JenkinsManager::GetJobStatus(QString job_name, int build_number){
    auto cache_name = GetJobStatusKey(job_name, build_number);
    auto current_configuration = getJobConfiguration(cache_name);

    Jenkins_Job_Status job_state;
    job_state.name = job_name;
    job_state.number = build_number;
    job_state.state = Notification::Severity::NONE;
    
    if(!current_configuration.isNull()){
        auto config = current_configuration.object();
        
        job_state.description  = config["description"].toString();
        

        for(auto j : config["actions"].toArray()){
            auto class_name = j.toObject()["_class"].toString();
            if(class_name == "hudson.model.CauseAction"){
                auto user_id = j.toObject()["causes"].toArray()[0].toObject()["userId"].toString();
                job_state.user_id = user_id;
                break;
            }
        }

        
        //Duration on complete is correct
        job_state.current_duration = config["duration"].toInt();


        //Get the array which contains the result.
        bool building = config["building"].toBool();
        if(building){
            job_state.state  = Notification::Severity::RUNNING;
        }else{
            QString result = config["result"].toString();
            if(result == "SUCCESS"){
                job_state.state  = Notification::Severity::SUCCESS;
            }else if(result == "ABORTED"){
                job_state.state = Notification::Severity::INFO;
            }else{
                job_state.state = Notification::Severity::ERROR;
            }
        }
    }
    return job_state;
}

QString JenkinsManager::GetJobStatusKey(const QString& job_name, int build_number){
    QString cached_name = job_name;

    //If we have been provided a valid build number, append this to the jobName
    if(build_number >= 0){
        cached_name += "/" + QString::number(build_number);
    }

    return cached_name;
}