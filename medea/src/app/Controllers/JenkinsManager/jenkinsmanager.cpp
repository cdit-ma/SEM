#include "jenkinsmanager.h"
#include "../ViewController/viewcontroller.h"
#include "../../Utils/filehandler.h"

#include <QApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QHttpMultiPart>
#include <QUrlQuery>

JenkinsManager::JenkinsManager(ViewController* view_controller)
	: QObject(view_controller),
	  var_mutex_(QMutex::Recursive)
{
    view_controller_ = view_controller;

    //Set scripts
    scripts_path_ = QApplication::applicationDirPath() + "/Resources/scripts/";

    auto settings = SettingsController::settings();
    connect(settings, &SettingsController::settingChanged, this, &JenkinsManager::SettingChanged);
    connect(settings, &SettingsController::settingsApplied, this, &JenkinsManager::ValidateSettings);
    connect(this, &JenkinsManager::SettingsValidated, view_controller, &ViewController::GotJenkins);

    //Sync settings to JenkinsManager
    for(auto key : settings->getSettingsKeys("Jenkins")){
        SettingChanged(key, settings->getSetting(key));
    }

    ValidateSettings();
}

QString JenkinsManager::GetUrl()
{
    QMutexLocker lock(&var_mutex_);
    return url_;
}

QString JenkinsManager::GetUser()
{
    QMutexLocker lock(&var_mutex_);
    return username_;
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

QString JenkinsManager::GetScriptsPath()
{
    QMutexLocker lock(&var_mutex_);
    return scripts_path_;
}

QString JenkinsManager::GetArtifactUrl(const QString& job_name, const int job_number, const QString& relative_path)
{
    return GetUrl() + "job/" + GetJobStatusKey(job_name, job_number) + "/artifact/" + relative_path;
}

void JenkinsManager::SetUrl(QString url)
{
    //Enforce a trailing forward-slash
    if(!url.endsWith("/")){
        url += "/";
    }

    QMutexLocker lock(&var_mutex_);
    if(url_ != url){
        url_ = url;
        SetSettingsDirty();
    }
}

void JenkinsManager::SetUser(const QString& user)
{
    QMutexLocker lock(&var_mutex_);
    if(username_ != user){
        username_ = user;
        SetSettingsDirty();
    }
}

void JenkinsManager::SetApiToken(const QString& api_token)
{
    QMutexLocker lock(&var_mutex_);
    if(token_ != api_token){
        token_ = api_token;
        SetSettingsDirty();
    }
}

void JenkinsManager::SetJobName(const QString& job_name)
{
    QMutexLocker lock(&var_mutex_);
    if(job_name_ != job_name){
        job_name_ = job_name;
        SetSettingsDirty();
    }
}

void JenkinsManager::SetSettingsDirty()
{
    QMutexLocker lock(&var_mutex_);
    settings_validated_ = false;
}

bool JenkinsManager::GotValidSettings()
{
    QMutexLocker lock(&var_mutex_);
    return settings_validated_;
}

bool JenkinsManager::WaitForSettingsValidation()
{
    auto validate_future = ValidateSettings().second;
    validate_future.waitForFinished();
    return GotValidSettings();
}

QPair<bool, QFuture<QString> > JenkinsManager::getJenkinsNodes()
{
    auto groovy_script = FileHandler::readTextFile(GetScriptsPath() + "Jenkins_Construct_GraphMLNodesList.groovy");
    return ExecuteGroovyScript("Get Jenkins Nodes", groovy_script);
}

QPair<bool, QFuture<QString> > JenkinsManager::ExecuteGroovyScript(const QString& title, const QString& groovy_script)
{
    auto future_key = GetUrl() + "Run_Groovy/" + title;
    QMutexLocker lock(&futures_mutex_);
    
    //Get the future used by the api_url (If we don't have one it will construct a finished QFuture)
    auto &future = string_futures_[future_key];
    bool made_request = false;

    if(GotValidSettings() && !future.isRunning()){
        future = QtConcurrent::run(this, &JenkinsManager::RequestExecuteGroovyScript, title, groovy_script);
        made_request = true;
    }

    return {made_request, future};
}

QString JenkinsManager::RequestExecuteGroovyScript(const QString& title, const QString& groovy_script)
{
    QString output;
    if(WaitForSettingsValidation()){
        auto notification = NotificationManager::manager()->AddNotification(title, "Icons", "jenkinsFlat", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::JENKINS);
        
        ProcessRunner runner;
        QUrlQuery script;
        
        //Encode the script
        script.addQueryItem("script", QUrl::toPercentEncoding(groovy_script));
        
        auto script_bytes = script.toString().toUtf8();
        auto api_request = getAuthenticatedRequest(GetUrl() + "scriptText");
        auto api_result = runner.HTTPPost(api_request, script_bytes);
        output = api_result.standard_output;
        notification->setSeverity(api_result.success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    }
    return output;
}

SETTING_TYPE JenkinsManager::GetSettingType(const QString &parameter_type)
{
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

QPair<bool, QFuture<Jenkins_Job_Status> > JenkinsManager::GetJobStatus(const QString& job_name, int job_number)
{
    auto hash_key = GetUrl() + GetJobStatusKey(job_name, job_number) + "/job_status";
    QMutexLocker lock(&futures_mutex_);
    
    //Get the future used by the api_url (If we don't have one it will construct a finished QFuture)
    auto &future = status_futures_[hash_key];
    bool made_request = false;

    //Don't allow requests once the the state is finished
    if(!future.isRunning() && ShouldRequestJob(job_name, job_number)){
        future = QtConcurrent::run(this, &JenkinsManager::RequestGetJobStatus, job_name, job_number);
        made_request = true;
    }

    return {made_request, future};
}

Jenkins_Job_Status JenkinsManager::RequestGetJobStatus(const QString& job_name, int job_number)
{
    //Get the Job Configuration
    auto configuration_future = GetJobConfiguration(job_name, job_number).second;
    //Wait for it to finish
    configuration_future.waitForFinished();
    return GetCachedJobStatus(job_name, job_number);
}

void JenkinsManager::GotoJob(const QString& job_name, int build_number)
{
    auto url = GetUrl() + "job/" + job_name + "/" + QString::number(build_number);
    view_controller_->openURL(url);
}

void JenkinsManager::SettingChanged(SETTINGS key, const QVariant& value)
{
    auto str_value = value.toString();

    switch(key){
    case SETTINGS::JENKINS_API:{
        SetApiToken(str_value);
        break;
    }
    case SETTINGS::JENKINS_JOBNAME:{
        SetJobName(str_value);
        break;
    }
    case SETTINGS::JENKINS_USER:{
        SetUser(str_value);
        break;
    }
    case SETTINGS::JENKINS_URL:{
        SetUrl(str_value);
        break;
    }
    case SETTINGS::JENKINS_TEST:{
        ValidateSettings();
        break;
    }
    default:
        break;
    }
}

QPair<bool, QFuture<bool> > JenkinsManager::ValidateSettings()
{
    auto hash_key = GetUrl() + "/api/json";
    QMutexLocker lock(&futures_mutex_);
    
    //Get the future used by the hash_key (If we don't have one it will construct a finished QFuture)
    auto &future = bool_futures_[hash_key];
    bool made_request = false;

    if(!GotValidSettings() && !future.isRunning()){
        future = QtConcurrent::run(this, &JenkinsManager::RequestValidation);
        made_request = true;
    }

    return {made_request, future};
}

QPair<bool, QFuture<QJsonDocument> > JenkinsManager::GetJenkinsConfiguration()
{
    auto hash_key = GetUrl() + "/api/json";
    QMutexLocker lock(&futures_mutex_);
    
    //Get the future used by the hash_key (If we don't have one it will construct a finished QFuture)
    auto got_result = json_futures_.contains(hash_key);
    auto &future = json_futures_[hash_key];
    bool made_request = false;

    if(!got_result && !future.isRunning()){
        future = QtConcurrent::run(this, &JenkinsManager::RequestJenkinsConfiguration, true);
        made_request = true;
    }

    return {made_request, future};
}

QJsonDocument JenkinsManager::RequestJenkinsConfiguration(bool auth)
{
    QJsonDocument document;
    auto api_url = GetUrl() + "/api/json";

    ProcessRunner runner;
    
    auto api_request = getAuthenticatedRequest(api_url, auth);
    auto api_result = runner.HTTPGet(api_request);

    if(api_result.success){
        //Get the JSON our request
        document = QJsonDocument::fromJson(api_result.standard_output.toUtf8());
        //Store the document
        if(auth){
            QMutexLocker lock(&cache_mutex_);
            cached_json_[api_url] = document;
        }
    }else{
        QJsonObject json_result;
        json_result.insert("ErrorCode", QJsonValue::fromVariant(api_result.error_code));
        document = QJsonDocument(json_result);
    }
    return document;
}

QPair<bool, QFuture<QJsonDocument> > JenkinsManager::GetJobConfiguration(const QString& job_name, int job_number)
{
    auto hash_key = GetUrl() + GetJobStatusKey(job_name, job_number) + "/api/json";
    QMutexLocker lock(&futures_mutex_);
    
    //Get the future used by the hash_key (If we don't have one it will construct a finished QFuture)
    auto &future = json_futures_[hash_key];
    bool made_request = false;

    //Only request if we aren't already running one, and we actually need to
    if(!future.isRunning() && ShouldRequestJob(job_name, job_number)){
        future = QtConcurrent::run(this, &JenkinsManager::RequestJobConfiguration, job_name, job_number);
        made_request = true;
    }

    return {made_request, future};
}

bool JenkinsManager::ShouldRequestJob(const QString& job_name, const int job_number)
{
    auto status = GetCachedJobStatus(job_name, job_number);
    return GotValidSettings() && (status.state == Notification::Severity::NONE || status.state == Notification::Severity::RUNNING);
}

Jenkins_Job_Status JenkinsManager::GetCachedJobStatus(const QString& job_name, const int job_number)
{
    auto hash_key = GetUrl() + "job/" + GetJobStatusKey(job_name, job_number) + "/api/json";

    Jenkins_Job_Status job_state;
    job_state.name = job_name;
    job_state.number = job_number;
    job_state.state = Notification::Severity::NONE;
    
    QMutexLocker lock(&cache_mutex_);
    if(cached_json_.contains(hash_key)){
        auto& json = cached_json_[hash_key];
        auto config = json.object();
        
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
        if(config.contains("building")){
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
        
        //Set the State for a top level job
        if(job_number == -1){
            job_state.state  = Notification::Severity::RUNNING;
        }
    }
    return job_state;
}

QJsonDocument JenkinsManager::RequestJobConfiguration(const QString& job_name, int job_number)
{
    QJsonDocument document;
    if(WaitForSettingsValidation()){
        ProcessRunner runner;
        auto api_url = GetUrl() + "job/" + GetJobStatusKey(job_name, job_number) + "/api/json";
        auto api_request = getAuthenticatedRequest(api_url);
        auto api_result = runner.HTTPGet(api_request);

        if(api_result.success){
            auto output = api_result.standard_output.toUtf8();
            document = QJsonDocument::fromJson(output);
            //Insert the cached_document into the map
            QMutexLocker lock(&cache_mutex_);
            cached_json_[api_url] = document;
        }
    }
    return document;
}

QPair<bool, QFuture<QList<Jenkins_Job_Parameter> > > JenkinsManager::GetJobParameters(const QString& job_name)
{
    auto hash_key = GetUrl() + GetJobStatusKey(job_name) + "/get_job_parameters";
    QMutexLocker lock(&futures_mutex_);
    
    //Get the future used by the api_url (If we don't have one it will construct a finished QFuture)
    auto got_result = parameter_futures_.contains(hash_key);
    auto &future = parameter_futures_[hash_key];
    bool made_request = false;

    //Only request if we aren't already running one, and we actually need to
    if(!future.isRunning() && !got_result){
        future = QtConcurrent::run(this, &JenkinsManager::RequestJobParameters, job_name);
        made_request = true;
    }

    return {made_request, future};
}

QList<Jenkins_Job_Parameter> JenkinsManager::RequestJobParameters(const QString& job_name)
{
    QList<Jenkins_Job_Parameter> job_parameters;

    //Request the job configuration
    auto json_future = GetJobConfiguration(job_name, -1).second;
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
        }else if(job_type == "org.jenkinsci.plugins.workflow.multibranch.WorkflowMultiBranchProject"){
            
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
            
            auto default_param = parameter_obj["defaultParameterValue"];

            if(default_param.isObject()){
                auto default_value = default_param.toObject()["value"];
                if(default_value.isBool()){
                    if(default_value.toBool()){
                        job_parameter.defaultValue = "true";
                    }else{
                        job_parameter.defaultValue = "false";
                    }
                }else if(default_value.isString()){
                    job_parameter.defaultValue = default_value.toString("");
                }
            }
            //Add it the jobParameter List
            job_parameters.append(job_parameter);
        }
    }

    return job_parameters;
}

QPair<bool, QFuture<bool> > JenkinsManager::BuildJob(const QString& job_name, const QList<Jenkins_Job_Parameter>& job_parameters)
{
    auto hash_key = GetUrl() + "/" + job_name + "/build";
    QMutexLocker lock(&futures_mutex_);
    
    //Get the future used by the api_url (If we don't have one it will construct a finished QFuture)
    auto &future = bool_futures_[hash_key];
    bool made_request = false;
    if(!future.isRunning()){
        future = QtConcurrent::run(this, &JenkinsManager::RequestBuildJob, job_name, job_parameters);
        made_request = true;
    }
    return {made_request, future};
}

bool JenkinsManager::RequestBuildJob(const QString& job_name, const QList<Jenkins_Job_Parameter>& job_parameters)
{
    bool result = false;
    auto notification = NotificationManager::manager()->AddNotification("Build Job '" + job_name + "'", "Icons", "jenkinsFlat", Notification::Severity::RUNNING, Notification::Type::MODEL, Notification::Category::JENKINS);
    
    if(WaitForSettingsValidation()){
        QScopedPointer<QHttpMultiPart> multi_part(new QHttpMultiPart(QHttpMultiPart::FormDataType));
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
                        file->setParent(multi_part.data());
                        multi_part->append(file_part);
                    }
                }
            }
        }

        ProcessRunner runner;
        auto build_url = GetUrl() + "job/" + job_name + "/build?delay=0sec";
        auto build_request = getAuthenticatedRequest(build_url);
        auto build_result = runner.HTTPPostMulti(build_request, multi_part.take());
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

    emit JobQueued(job_name);
    return result;
}

QPair<bool, QFuture<QString> > JenkinsManager::GetJobConsoleOutput(const QString& job_name, int job_number)
{
    auto hash_key = GetUrl() + "job/" + job_name + "/" + QString::number(job_number) + "/consoleText";
    QMutexLocker lock(&futures_mutex_);
    
    //Get the future used by the api_url (If we don't have one it will construct a finished QFuture)
    auto &future = string_futures_[hash_key];
    bool made_request = false;

    if(GotValidSettings() && !future.isRunning()){
        future = QtConcurrent::run(this, &JenkinsManager::RequestJobConsoleOutput, job_name, job_number);
        made_request = true;
    }

    return {made_request, future};
}

QString JenkinsManager::RequestJobConsoleOutput(const QString& job_name, int job_number)
{
    QString console_output;
    if(WaitForSettingsValidation()){
        ProcessRunner runner;
        auto api_url = GetUrl() + "job/" + GetJobStatusKey(job_name, job_number) + "/consoleText";
        auto api_request = getAuthenticatedRequest(api_url);
        auto api_result = runner.HTTPGet(api_request);

        if(api_result.success){
            console_output = api_result.standard_output;
        }
    }
    return console_output;
}

QPair<bool, QFuture<bool> > JenkinsManager::AbortJob(const QString& job_name, int job_number)
{
    auto hash_key = GetUrl() + "job/" + job_name + "/" + QString::number(job_number) + "/stop";
    QMutexLocker lock(&futures_mutex_);
    
    //Get the future used by the api_url (If we don't have one it will construct a finished QFuture)
    auto &future = bool_futures_[hash_key];
    bool made_request = false;

    if(GotValidSettings() && !future.isRunning()){
        future = QtConcurrent::run(this, &JenkinsManager::RequestAbortJob, job_name, job_number);
        made_request = true;
    }

    return {made_request, future};
}

bool JenkinsManager::RequestAbortJob(const QString& job_name, int job_number)
{
    bool success = false;
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
    return success;
}

bool JenkinsManager::RequestValidation()
{
    //Construct a Notification for validating
    auto notification = NotificationManager::manager()->AddNotification("Validating Jenkins Settings", "Icons", "jenkinsFlat", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::JENKINS);
    
    //Test Location of server
    auto unvalidated_json = RequestJenkinsConfiguration(false);

    auto config = unvalidated_json.object();

    bool result = false;
    QString result_string;
    
    if(!config.contains("ErrorCode")){
        //Request a Validated Jenkins Configuration
        auto request_configuration = GetJenkinsConfiguration();
        //Block for results
        auto validated_json = request_configuration.second.result();
        //Updated our config
        config = validated_json.object();
    }

    if(config.contains("ErrorCode")){
        auto error = (QNetworkReply::NetworkError) config["ErrorCode"].toInt();
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
    }else{
        auto required_job_name = GetJobName();

        auto got_job = false;

        //Check through the list of jobs to 
        for(auto job : config["jobs"].toArray()){
            auto job_name = job.toObject()["name"].toString();
            if(job_name == required_job_name){
                got_job = true;
                break;
            }
        }
        
        if(got_job){
            result = true;
        }else{
            result_string = "Doesn't have Job named: '" + required_job_name + "'";
        }
    }

    //Update the notification
    notification->setSeverity(result ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    notification->setTitle(result ? "Jenkins settings validated successfully": "Jenkins settings invalid");
    notification->setDescription(result_string);
    SetSettingsValidated(result);

    return result;
}

void JenkinsManager::SetSettingsValidated(bool valid)
{
	ClearFutures();
	ClearCache();
	
	QMutexLocker var_lock(&var_mutex_);
	settings_validated_ = valid;
	emit SettingsValidated(settings_validated_);
}

void JenkinsManager::ClearFutures()
{
	//Clear all futures
	QMutexLocker future_lock(&futures_mutex_);
	json_futures_.clear();
	bool_futures_.clear();
	string_futures_.clear();
	parameter_futures_.clear();
	status_futures_.clear();
}

void JenkinsManager::ClearCache()
{
	QMutexLocker cache_lock(&cache_mutex_);
	cached_json_.clear();
}

QNetworkRequest JenkinsManager::getAuthenticatedRequest(const QString& url, bool auth)
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

QString JenkinsManager::GetJobStatusKey(const QString& job_name, const int build_number)
{
    QString cached_name = job_name;

    //If we have been provided a valid build number, append this to the jobName
    if(build_number >= 0){
        cached_name += "/" + QString::number(build_number);
    }

    return cached_name;
}
