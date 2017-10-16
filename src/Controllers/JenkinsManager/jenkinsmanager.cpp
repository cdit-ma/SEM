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

JenkinsManager::JenkinsManager(ViewController* view_controller) : QObject(view_controller)
{
    //Register the Types used as parameters JenkinsRequest so signals/slots can be connected.
    qRegisterMetaType<Jenkins_JobParameters>("Jenkins_JobParameters");

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


    //connect(GetJobMonitorWidget(), &JenkinsJobMonitorWidget::gotoURL, view_controller, &ViewController::openURL);
    connect(this, &JenkinsManager::GotJenkinsNodes, view_controller, &ViewController::jenkinsManager_GotJenkinsNodesList);
    connect(this, &JenkinsManager::JenkinsReady, view_controller, &ViewController::GotJenkins);

    connect(view_controller, &ViewController::vc_executeJenkinsJob, this, &JenkinsManager::BuildJob);

    auto action_controller = view_controller->getActionController();
    connect(action_controller->jenkins_importNodes, &QAction::triggered, this, &JenkinsManager::GetNodes);

    //Validate
    ValidateSettings();
}

QString JenkinsManager::GetUser()
{
    QMutexLocker(&this->mutex_);
    return username_;
}

void JenkinsManager::SetUrl(QString url)
{
    //Enforce a trailing /
    if(!url.endsWith("/")){
        url += "/";
    }

    QMutexLocker(&this->mutex_);
    if(url_ != url){
        url_ = url;
        settings_changed = true;
        settings_validated_ = false;
    }
}

void JenkinsManager::SetUser(QString user)
{
    QMutexLocker(&this->mutex_);
    if(username_ != user){
        username_ = user;
        settings_changed = true;
        settings_validated_ = false;
    }
}

void JenkinsManager::SetApiToken(QString api_token)
{
    QMutexLocker(&this->mutex_);
    if(token_ != api_token){
        token_ = api_token;
        settings_changed = true;
        settings_validated_ = false;
    }
}

void JenkinsManager::SetJobName(QString job_name)
{
    QMutexLocker(&this->mutex_);
    if(job_name_ != job_name){
        job_name_ = job_name;
        settings_changed = true;
        settings_validated_ = false;
    }
}

bool JenkinsManager::GotValidSettings()
{
    QMutexLocker(&this->mutex_);
    return settings_validated_;
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

    QMutexLocker(&this->mutex_);
    requests.append(request);
    return request;
}

void JenkinsManager::GetNodes()
{
    QMutexLocker lock(&mutex_);
    //Check if we need to validate
    auto script = FileHandler::readTextFile(scipts_path_ + "Jenkins_Construct_GraphMLNodesList.groovy");
    auto can_run = settings_validated_ && script.length();

    if(can_run && !get_nodes_thread.isRunning()){
        get_nodes_thread = QtConcurrent::run(this, &JenkinsManager::GetNodes_, script);
    }
}

SETTING_TYPE JenkinsManager::GetSettingType(QString parameter_type){
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

            bool got_model = false;
            for(auto parameter : parameters){
                //Ignore model
                if(parameter.name == "model"){
                    got_model = true;
                    continue;
                }
                auto type = GetSettingType(parameter.type);
                dialog.addOption(parameter.name, type, parameter.defaultValue);
                dialog.setOptionIcon(parameter.name, "Icons", "label");
            }

            auto options = dialog.getOptions();

            auto got_options = got_model ? options.size() + 1 == parameters.size() : options.size() == parameters.size();
            //
            if(got_options){
                Jenkins_JobParameters build_parameters;
                for(auto parameter_name : options.keys()){
                    Jenkins_Job_Parameter parameter;
                    parameter.name = parameter_name;
                    parameter.value = options.value(parameter_name).toString();
                    build_parameters += parameter;
                }
                if(got_model){
                    Jenkins_Job_Parameter parameter;
                    parameter.name = "model";
                    parameter.value = FileHandler::readTextFile(model_file);
                    build_parameters += parameter;
                }

                auto jenkins_build = GetJenkinsRequest();
            
                auto build_job = connect(this, &JenkinsManager::buildJob, jenkins_build, &JenkinsRequest::BuildJob);
                
                emit buildJob(job_name_, build_parameters);
                disconnect(build_job);

                connect(jenkins_build, &JenkinsRequest::GotJobStateChange, this, &JenkinsManager::gotJobStateChange);
                connect(jenkins_build, &JenkinsRequest::GotLiveJobConsoleOutput, this, &JenkinsManager::gotJobConsoleOutput);
            }
        }, Qt::QueuedConnection);
        
        emit getJobParameters(job_name_);
        disconnect(r);
    }
}


void JenkinsManager::gotoJob(QString job_name, int build_number){
    auto url = GetUrl() + "job/" + job_name + "/" + QString::number(build_number);
    view_controller_->openURL(url);
}


void JenkinsManager::AbortJob(QString job_name, int job_number){
    if(GotValidSettings()){
        QtConcurrent::run(this, &JenkinsManager::AbortJob_, job_name, job_number);
    }
}

void JenkinsManager::gotJobStateChange(QString job_name, int job_build, QString activeConfiguration, Notification::Severity job_state){
    JenkinsMonitor* jenkins_monitor = 0;
    auto monitor = view_controller_->getExecutionMonitor();
    if(monitor){
        //First off
        if(job_state == Notification::Severity::RUNNING){
            view_controller_->showExecutionMonitor();
            jenkins_monitor = monitor->constructJenkinsMonitor(job_name, job_build);
            connect(jenkins_monitor, &JenkinsMonitor::Abort, [=](){AbortJob_(job_name, job_build);});
            connect(jenkins_monitor, &JenkinsMonitor::GotoURL, [=](){gotoJob(job_name, job_build);});
        }else{
            jenkins_monitor = (JenkinsMonitor*)monitor->getJenkinsMonitor(job_name, job_build);
        }


        if(jenkins_monitor){
            jenkins_monitor->StateChanged(job_state);
        }
    }
}

void JenkinsManager::gotJobConsoleOutput(QString job_name, int job_build, QString activeConfiguration, QString consoleOutput){
    auto monitor = view_controller_->getExecutionMonitor();
    if(monitor){
        auto jenkins_monitor = monitor->getJenkinsMonitor(job_name, job_build);
        if(jenkins_monitor){
            jenkins_monitor->AppendLine(consoleOutput);
        }
    }
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
/**
 * @brief JenkinsManager::validateJenkinsSettings Constructs a JenkinsRequest Thread to validate the Jenkins Settings.
 */
void JenkinsManager::ValidateSettings()
{
    QMutexLocker lock(&mutex_);
    //Check if we need to validate
    auto requires_validation = !settings_validated_;
    if(requires_validation && !validation_thread.isRunning()){
        validation_thread = QtConcurrent::run(this, &JenkinsManager::ValidateSettings_);
    }
}

void JenkinsManager::AbortJob_(QString job_name, int build_number){
    if(GotValidSettings()){
        auto build_number_str = QString::number(build_number);
        auto notification = NotificationManager::manager()->AddNotification("Stopping Jenkins Job '" + job_name + "' #" + build_number_str, "Icons", "jenkinsFlat", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::JENKINS);
        
        ProcessRunner runner;
        auto api_url = GetUrl() + "job/" + job_name + "/" + build_number_str + "/stop";
        auto api_request = getAuthenticatedRequest(api_url);
        auto api_result = runner.HTTPPost(api_request);

        notification->setTitle(api_result.success ? "Stopped Jenkins Job '" + job_name + "' #" + build_number_str : "Failed to stop Jenkins Job '" + job_name + "' #" + build_number_str);
        notification->setSeverity(api_result.success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    }
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

void JenkinsManager::ValidateSettings_(){
    auto notification = NotificationManager::manager()->AddNotification("Validating Jenkins Settings", "Icons", "jenkinsFlat", Notification::Severity::RUNNING, Notification::Type::APPLICATION, Notification::Category::JENKINS);

    auto api_url = GetUrl() + "api/json";
    auto api_request = getAuthenticatedRequest(api_url, false);

    ProcessRunner runner;
    
    bool success = false;
    QString result_string;

    //Run an un-authed api call first
    auto api_result = runner.HTTPGet(api_request);

    //If we succeeded, re-run an authorized command
    if(api_result.success){
        auto api_auth_request = getAuthenticatedRequest(api_url, true);
        api_result = runner.HTTPGet(api_auth_request);
    }


    if(api_result.success){
        //Check the JSON for the job
        auto json = QJsonDocument::fromJson(api_result.standard_output.toUtf8()).object();

        auto required_job_name = GetJobName();

        auto got_job = false;

        for(auto j : json["jobs"].toArray()){
            auto job_name = j.toObject()["name"].toString();
            if(job_name == required_job_name){
                got_job = true;
                break;
            }
        }
        
        if(got_job){
            success = true;
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

    //Update the state
    notification->setSeverity(success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
    notification->setTitle(success ? "Jenkins settings validated successfully": "Jenkins settings invalid");
    if(!success){
        notification->setDescription(result_string);
    }

    {
        //Set the state
        QMutexLocker lock(&mutex_);
        settings_validated_ = success;
        //Clear the settings_changed flag
        settings_changed = false;
        //Remove the jobs json cache
        jobsJSON.clear();
    }

    emit JenkinsReady(settings_validated_);
}


void JenkinsManager::jenkinsRequestFinished(JenkinsRequest *request)
{
    requests.removeAll(request);
}

QString JenkinsManager::GetUrl()
{
    QMutexLocker(&this->mutex_);
    return url_;
}

void JenkinsManager::storeJobConfiguration(QString jobName, QJsonDocument json)
{
    QMutexLocker(&this->mutex_);
    jobsJSON[jobName] = json;
}

QNetworkRequest JenkinsManager::getAuthenticatedRequest(QString url, bool auth)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    //Attach Headers
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
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
    QMutexLocker(&this->mutex_);
    return jobsJSON.value(job_name, QJsonDocument());
}

QString JenkinsManager::GetJobName()
{
    QMutexLocker(&this->mutex_);
    return job_name_;
}

QString JenkinsManager::GetApiToken()
{
    QMutexLocker(&this->mutex_);
    return token_;
}

