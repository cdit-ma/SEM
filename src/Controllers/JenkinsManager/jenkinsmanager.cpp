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
    connect(settings, &SettingsController::settingsApplied, this, &JenkinsManager::ValidateSettings);

    //Sync settings to JenkinsManager
    foreach(SETTINGS key, settings->getSettingsKeys("Jenkins")){
        SettingChanged(key, settings->getSetting(key));
    }


    //connect(GetJobMonitorWidget(), &JenkinsJobMonitorWidget::gotoURL, view_controller, &ViewController::openURL);
    connect(this, &JenkinsManager::gotValidSettings, view_controller, &ViewController::jenkinsManager_SettingsValidated);
    connect(this, &JenkinsManager::GotJenkinsNodes, view_controller, &ViewController::jenkinsManager_GotJenkinsNodesList);
    connect(this, &JenkinsManager::JenkinsReady, view_controller, &ViewController::vc_JenkinsReady);

    connect(view_controller, &ViewController::vc_executeJenkinsJob, this, &JenkinsManager::BuildJob);

    auto action_controller = view_controller->getActionController();
    connect(action_controller->jenkins_importNodes, &QAction::triggered, this, &JenkinsManager::GetNodes);

    //Validate
    ValidateSettings();
}

ActionController *JenkinsManager::GetActionController()
{
    return view_controller_->getActionController();
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
        settings_validated_ = false;
    }
}

void JenkinsManager::SetUser(QString user)
{
    QMutexLocker(&this->mutex_);
    if(username_ != user){
        username_ = user;
        settings_validated_ = false;
    }
}

void JenkinsManager::SetApiToken(QString api_token)
{
    QMutexLocker(&this->mutex_);
    if(token_ != api_token){
        token_ = api_token;
        settings_validated_ = false;
    }
}

void JenkinsManager::SetJobName(QString job_name)
{
    QMutexLocker(&this->mutex_);
    if(job_name_ != job_name){
        job_name_ = job_name;
        settings_validated_ = false;
    }
}

bool JenkinsManager::HasSettings()
{
    QMutexLocker(&this->mutex_);
    return !(token_.isEmpty() || username_.isEmpty() || url_.isEmpty() || job_name_.isEmpty());
}

bool JenkinsManager::GotValidSettings()
{
    QMutexLocker(&this->mutex_);
    return settings_validated_;
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
    QMutexLocker(&this->mutex_);
    if(!requesting_nodes_){
        //Set the requesting nodes
        requesting_nodes_ = true;
        auto request = GetJenkinsRequest();
        
        jenkins_request_noti = NotificationManager::manager()->AddNotification("Requesting Jenkins Nodes", "Icons", "jenkinsFlat", Notification::Severity::RUNNING, Notification::Type::MODEL, Notification::Category::JENKINS);
        connect(request, &JenkinsRequest::GotGroovyScriptOutput, this, &JenkinsManager::GotJenkinsNodes_);
        auto r = connect(this, &JenkinsManager::RunGroovyScript_, request, &JenkinsRequest::RunGroovyScript);

        //Read in the contents of the script
        auto script = FileHandler::readTextFile(scipts_path_ + "Jenkins_Construct_GraphMLNodesList.groovy");
        emit RunGroovyScript_(script);
        disconnect(r);
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


void JenkinsManager::abortJob(QString job_name, int build_number){
    auto jenkins_stop = GetJenkinsRequest(this);
    auto r = connect(this, &JenkinsManager::AbortJob, jenkins_stop, &JenkinsRequest::StopJob);
    emit AbortJob(job_name, build_number);
    disconnect(r);
}

void JenkinsManager::gotJobStateChange(QString job_name, int job_build, QString activeConfiguration, Notification::Severity job_state){
    JenkinsMonitor* jenkins_monitor = 0;
    auto monitor = view_controller_->getExecutionMonitor();
    if(monitor){
        //First off
        if(job_state == Notification::Severity::RUNNING){
            view_controller_->showExecutionMonitor();
            jenkins_monitor = monitor->constructJenkinsMonitor(job_name, job_build);
            connect(jenkins_monitor, &JenkinsMonitor::Abort, [=](){abortJob(job_name, job_build);});
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

void JenkinsManager::GotJenkinsNodes_(bool success, QString data)
{
    QMutexLocker(&this->mutex_);
    requesting_nodes_ = false;

    if(success){
        emit GotJenkinsNodes(data);
    }
    if(jenkins_request_noti){
        jenkins_request_noti->setDescription(success ? "Successfully requested Jenkins nodes" : "Failed to request Jenkins nodes");
        jenkins_request_noti->setSeverity(success ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
        jenkins_request_noti = 0;
    }
}

void JenkinsManager::SettingChanged(SETTINGS key, QVariant value)
{
    QString strValue = value.toString();

    switch(key){
    case SETTINGS::JENKINC_API:{
        SetApiToken(strValue);
        break;
    }
    case SETTINGS::JENKINC_JOBNAME:{
        SetJobName(strValue);
        break;
    }
    case SETTINGS::JENKINC_USER:{
        SetUser(strValue);
        break;
    }
    case SETTINGS::JENKINC_URL:{
        SetUrl(strValue);
        break;
    }
    default:
        break;
    }
}

void JenkinsManager::GotValidatedSettings_(bool valid, QString message)
{
    QMutexLocker(&this->mutex_);
    settings_validating_ = false;
    settings_validated_ = valid;
    //Clear on settings changed
    jobsJSON.clear();

    //Emit signal to all JenkinsRequests
    emit gotValidSettings(valid, message);
    emit JenkinsReady(settings_validated_);
}

/**
 * @brief JenkinsManager::validateJenkinsSettings Constructs a JenkinsRequest Thread to validate the Jenkins Settings.
 */
void JenkinsManager::ValidateSettings()
{
    QMutexLocker(&this->mutex_);
    if(!settings_validating_ && !settings_validated_){
        emit JenkinsReady(settings_validated_);
        //Validate Settings
        settings_validating_ = true;
        JenkinsRequest* request = GetJenkinsRequest();
        auto r = connect(this, &JenkinsManager::ValidateSettings_, request, &JenkinsRequest::ValidateSettings);
        connect(request, &JenkinsRequest::GotValidatedSettings, this, &JenkinsManager::GotValidatedSettings_);

        emit ValidateSettings_();
        disconnect(r);
    }
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

QStringList JenkinsManager::GetJobConfigurations(QString job_name)
{
    auto json = getJobConfiguration(job_name).object();
    QStringList configs;

    if(!json.isEmpty()){
        //Blank for Master
        configs += "";

        //Append The name of the Active Configurations to the configurationList
        foreach(QJsonValue activeConfiguration, json["activeConfigurations"].toArray()){
            configs.append(activeConfiguration.toObject()["name"].toString());
        }
    }
    return configs;
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

