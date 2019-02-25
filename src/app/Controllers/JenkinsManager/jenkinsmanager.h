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
    JenkinsManager(ViewController *viewController);

    void SetUrl(QString url);
    void SetUser(QString user);
    void SetApiToken(QString api_token);
    void SetJobName(QString job_name);
    bool GotValidSettings();
    
    QString GetUrl();
    QString GetUser();
    QString GetJobName();
    QString GetApiToken();
    QString GetScriptsPath();

    QString GetArtifactUrl(const QString& job_name, const int job_number, const QString& relative_path);

    Jenkins_Job_Status GetCachedJobStatus(const QString& job_name, const int job_number);
    void GotoJob(QString job_name, int job_number);

    QPair<bool, QFuture<bool> > ValidateSettings();
    QPair<bool, QFuture<QJsonDocument> > GetJenkinsConfiguration();

    QPair<bool, QFuture<bool> > BuildJob(QString job_name, QList<Jenkins_Job_Parameter> job_parameters);
    QPair<bool, QFuture<bool> > AbortJob(QString job_name, int job_number);
    
    QPair<bool, QFuture<QString> > getJenkinsNodes();
    QPair<bool, QFuture<QString> > ExecuteGroovyScript(QString title, QString script);
    QPair<bool, QFuture<QString> > GetJobConsoleOutput(QString job_name, int job_number);

    QPair<bool, QFuture<QJsonDocument> > GetJobConfiguration(QString job_name, int job_number = -1);
    QPair<bool, QFuture<QList<Jenkins_Job_Parameter> > > GetJobParameters(QString job_name);
    QPair<bool, QFuture<Jenkins_Job_Status> > GetJobStatus(QString job_name, int job_number);
signals:
    void Terminate();
    void SettingsValidated(bool ready);
    void JobQueued(QString job_name);
private:
    //Request Functions are designed to be called by async threads
    bool RequestValidation();
    bool RequestBuildJob(QString job_name, QList<Jenkins_Job_Parameter> job_parameters);
    bool RequestAbortJob(QString job_name, int job_number);

    
    QString RequestJobConsoleOutput(QString job_name, int job_number);
    QString RequestExecuteGroovyScript(QString title, QString script);
    
    QJsonDocument RequestJenkinsConfiguration(bool auth = true);
    QJsonDocument RequestJobConfiguration(QString job_name, int job_number = -1);
    Jenkins_Job_Status RequestGetJobStatus(QString job_name, int job_number);
    QList<Jenkins_Job_Parameter> RequestJobParameters(QString job_name);

    void SetSettingsDirty();
    void SetSettingsValidated(bool valid);
    bool WaitForSettingsValidation();
    bool ShouldRequestJob(const QString& job_name, const int job_number);
private:
    void SettingChanged(SETTINGS key, QVariant value);

    static QString GetJobStatusKey(const QString& job_name, const int job_number = -1);
    static SETTING_TYPE GetSettingType(const QString& type);
    QNetworkRequest getAuthenticatedRequest(const QString& url, bool auth=true);

    //Mutex used by all getters/setters (recursive)
    QMutex var_mutex_;

    //Instance Variables
    QString url_;
    QString username_;
    QString token_;
    QString scripts_path_;
    QString job_name_;

    //Settings Validation Flags
    bool settings_dirty_ = false;
    bool settings_validated_ = false;

    //Hash to store futures
    QMutex futures_mutex_;
    QHash<QString, QFuture<QJsonDocument> > json_futures_;
    QHash<QString, QFuture<bool> > bool_futures_;
    QHash<QString, QFuture<QString> > string_futures_;
    QHash<QString, QFuture<QList<Jenkins_Job_Parameter> > > parameter_futures_;
    QHash<QString, QFuture<Jenkins_Job_Status> > status_futures_;
    
    //Hash to cache json configurations scraped
    QMutex cache_mutex_;
    QHash<QString, QJsonDocument> cached_json_;

    ViewController* view_controller_ = 0;
};
#endif // JENKINSMANAGER_H
