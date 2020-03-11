#ifndef JENKINSMANAGER_H
#define JENKINSMANAGER_H

#include "../SettingsController/settingscontroller.h"
#include "../NotificationManager/notificationenumerations.h"

#include <QObject>
#include <QJsonDocument>
#include <QString>
#include <QNetworkReply>
#include <QHash>
#include <QMutex>
#include <QFuture>

class ViewController;

struct Jenkins_Job_Parameter{
    QString name;
    QString value;
    SETTING_TYPE type;
    QString description;
    QString defaultValue;
};

struct Jenkins_Job_Status{
    QString name;
    int number;
    Notification::Severity state;
    QString description;
    QString user_id;
    int current_duration = -1;
};

class JenkinsManager : public QObject
{
    Q_OBJECT
    
public:
    explicit JenkinsManager(ViewController *viewController);

    void SetUrl(QString url);
    void SetUser(const QString& user);
    void SetApiToken(const QString& api_token);
    void SetJobName(const QString& job_name);
    bool GotValidSettings();
    
    QString GetUrl();
    QString GetUser();
    QString GetJobName();
    QString GetApiToken();
    QString GetScriptsPath();

    QString GetArtifactUrl(const QString& job_name, int job_number, const QString& relative_path);

    Jenkins_Job_Status GetCachedJobStatus(const QString& job_name, int job_number);
    void GotoJob(const QString& job_name, int job_number);

    QPair<bool, QFuture<bool> > ValidateSettings();
    QPair<bool, QFuture<QJsonDocument> > GetJenkinsConfiguration();

    QPair<bool, QFuture<bool> > BuildJob(const QString& job_name, const QList<Jenkins_Job_Parameter>& job_parameters);
    QPair<bool, QFuture<bool> > AbortJob(const QString& job_name, int job_number);
    
    QPair<bool, QFuture<QString> > getJenkinsNodes();
    QPair<bool, QFuture<QString> > ExecuteGroovyScript(const QString& title, const QString& script);
    QPair<bool, QFuture<QString> > GetJobConsoleOutput(const QString& job_name, int job_number);

    QPair<bool, QFuture<QJsonDocument> > GetJobConfiguration(const QString& job_name, int job_number = -1);
    QPair<bool, QFuture<QList<Jenkins_Job_Parameter> > > GetJobParameters(const QString& job_name);
    QPair<bool, QFuture<Jenkins_Job_Status> > GetJobStatus(const QString& job_name, int job_number);
    
signals:
    void Terminate();
    void SettingsValidated(bool ready);
    void JobQueued(QString job_name);
    
private:
    //Request Functions are designed to be called by async threads
    bool RequestValidation();
    bool RequestBuildJob(const QString& job_name, const QList<Jenkins_Job_Parameter>& job_parameters);
    bool RequestAbortJob(const QString& job_name, int job_number);

    QString RequestJobConsoleOutput(const QString& job_name, int job_number);
    QString RequestExecuteGroovyScript(const QString& title, const QString& groovy_script);
    
    QJsonDocument RequestJenkinsConfiguration(bool auth = true);
    QJsonDocument RequestJobConfiguration(const QString& job_name, int job_number = -1);
    Jenkins_Job_Status RequestGetJobStatus(const QString& job_name, int job_number);
    QList<Jenkins_Job_Parameter> RequestJobParameters(const QString& job_name);

    void SetSettingsDirty();
    void SetSettingsValidated(bool valid);
    bool WaitForSettingsValidation();
    bool ShouldRequestJob(const QString& job_name, int job_number);

    void SettingChanged(SETTINGS key, const QVariant& value);

    static QString GetJobStatusKey(const QString& job_name, int job_number = -1);
    static SETTING_TYPE GetSettingType(const QString& type);
    QNetworkRequest getAuthenticatedRequest(const QString& url, bool auth = true);

    //Mutex used by all getters/setters (recursive)
    QMutex var_mutex_;

    //Instance Variables
    QString url_;
    QString username_;
    QString token_;
    QString scripts_path_;
    QString job_name_;

    //Settings Validation Flags
    bool settings_validated_ = false;

    //Hash to store futures
    QHash<QString, QFuture<QJsonDocument> > json_futures_;
    QMutex futures_mutex_;
    QHash<QString, QFuture<bool> > bool_futures_;
    QHash<QString, QFuture<QString> > string_futures_;
    QHash<QString, QFuture<QList<Jenkins_Job_Parameter> > > parameter_futures_;
    QHash<QString, QFuture<Jenkins_Job_Status> > status_futures_;
    
    //Hash to cache json configurations scraped
    QMutex cache_mutex_;
    QHash<QString, QJsonDocument> cached_json_;

    ViewController* view_controller_ = nullptr;
};

#endif // JENKINSMANAGER_H
