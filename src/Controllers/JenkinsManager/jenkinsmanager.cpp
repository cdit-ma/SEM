#include "jenkinsmanager.h"
#include "jenkinsrequest.h"

#include "../ViewController/viewcontroller.h"
#include "../ActionController/actioncontroller.h"
#include "../NotificationManager/notificationmanager.h"
#include "../../Utils/filehandler.h"
#include "../../Widgets/Jenkins/jenkinsjobmonitorwidget.h"
#include "../../Widgets/Jenkins/jenkinsstartjobwidget.h"

#include <QStringBuilder>
#include <QApplication>
#include <QJsonArray>
#include <QJsonObject>

JenkinsManager::JenkinsManager(ViewController* view_controller) : QObject(view_controller)
{
    //Register the Types used as parameters JenkinsRequest so signals/slots can be connected.
    qRegisterMetaType<Jenkins_JobParameters>("Jenkins_JobParameters");
    qRegisterMetaType<JOB_STATE>("JOB_STATE");

    view_controller_ = view_controller;
    //Set script directory
    scipts_path_ = QApplication::applicationDirPath() % "/Resources/scripts/";

    //Connect to settings changes
    SettingsController* settings = SettingsController::settings();
    connect(settings, &SettingsController::settingChanged, this, &JenkinsManager::SettingChanged);
    connect(settings, &SettingsController::settingsApplied, this, &JenkinsManager::ValidateSettings);

    //Sync settings to JenkinsManager
    foreach(SETTING_KEY key, settings->getSettingsKeys("Jenkins")){
        SettingChanged(key, settings->getSetting(key));
    }


    connect(GetJobMonitorWidget(), &JenkinsJobMonitorWidget::gotoURL, view_controller, &ViewController::openURL);
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

JenkinsJobMonitorWidget *JenkinsManager::GetJobMonitorWidget()
{
    if(!job_monitor_widget_){
         job_monitor_widget_ = new JenkinsJobMonitorWidget(0, this);
    }
    return job_monitor_widget_;
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

        connect(request, &JenkinsRequest::GotGroovyScriptOutput, this, &JenkinsManager::GotJenkinsNodes_);
        auto r = connect(this, &JenkinsManager::RunGroovyScript_, request, &JenkinsRequest::RunGroovyScript);

        //Read in the contents of the script
        auto script = FileHandler::readTextFile(scipts_path_ + "Jenkins_Construct_GraphMLNodesList.groovy");
        emit RunGroovyScript_(script);
        disconnect(r);
    }
}

void JenkinsManager::BuildJob(QString model_file)
{
    if(GotValidSettings()){
        JenkinsStartJobWidget* jenkinsSJ = new JenkinsStartJobWidget(job_name_, this);
        jenkinsSJ->requestJob(job_name_, model_file);
    }
}

void JenkinsManager::GotJenkinsNodes_(bool success, QString data)
{
    QMutexLocker(&this->mutex_);
    requesting_nodes_ = false;

    emit NotificationManager::manager()->backgroundProcess(false, BACKGROUND_PROCESS::IMPORT_JENKINS);

    if(success){
        emit GotJenkinsNodes(data);
        NotificationManager::manager()->displayNotification("Successfully requested Jenkins Nodes", "Icons", "jenkins", -1, NOTIFICATION_SEVERITY::INFO, NOTIFICATION_TYPE::MODEL, NOTIFICATION_CATEGORY::JENKINS);
    }else{
        NotificationManager::manager()->displayNotification("Failed to request Jenkins Nodes", "Icons", "jenkins", -1, NOTIFICATION_SEVERITY::ERROR, NOTIFICATION_TYPE::MODEL, NOTIFICATION_CATEGORY::JENKINS);
    }
}

void JenkinsManager::SettingChanged(SETTING_KEY key, QVariant value)
{
    QString strValue = value.toString();

    switch(key){
    case SK_JENKINC_API:{
        SetApiToken(strValue);
        break;
    }
    case SK_JENKINC_JOBNAME:{
        SetJobName(strValue);
        break;
    }
    case SK_JENKINC_USER:{
        SetUser(strValue);
        break;
    }
    case SK_JENKINC_URL:{
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

