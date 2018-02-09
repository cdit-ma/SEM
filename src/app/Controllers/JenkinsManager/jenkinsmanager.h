#ifndef JENKINSMANAGER_H
#define JENKINSMANAGER_H

#include "../SettingsController/settingscontroller.h"

#include "../../Controllers/JenkinsManager/jenkinsrequest.h"


#include <QObject>
#include <QJsonDocument>
#include <QString>
#include <QVector>
#include <QHash>
#include <QNetworkReply>
#include <QMutex>
#include <QFuture>
#include <QFutureWatcher>

//Forward Declare
class JenkinsRequest;
class ActionController;
class ViewController;

class NotificationObject;

class JenkinsManager: public QObject
{
    //Define Jenkins Request as friend to enable use of private methods
    friend class JenkinsRequest;

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

//Global Functions Refactored
    void ValidateSettings();
    void GetNodes();
    void BuildJob(QString model_file);
    void AbortJob(QString job_name, int job_number);
    void GetJobConsoleOutput(QString job_name, int job_number);
    void GetRecentJobs(QString job_name);
    void GotoJob(QString job_name, int build_number);

    Jenkins_Job_Status GetJobStatus(QString job_name, int build_number);
signals:
    void BuildingJob(QString job_name);

    void Terminate();
    
    void GotJenkinsNodes(QString data);
    void JenkinsReady(bool ready);

    void getJobParameters(QString name);
    void buildJob(QString jobName, Jenkins_JobParameters parameters);
    void getJobConsole(QString job_name, int job_number);
    void gotRecentJobs(Jenkins_Job_Statuses recent_jobs);


    void getRecentJobs(QString job_name, int max_request_count, bool only_by_user);

    void gotJobArtifacts(QString job_name, int job_build, QStringList artifacts);
    void gotJobStateChange(QString job_name, int job_build, Notification::Severity jobState);
    void gotJobConsoleOutput(QString job_name, int job_build, QString consoleOutput);
private slots:
    void settingsApplied();
    void SettingChanged(SETTINGS key, QVariant value);

    

  
private:
    static QString GetJobStatusKey(const QString& jobName, int buildNumber);
    QNetworkRequest getAuthenticatedRequest(QString url, bool auth=true);


    JenkinsRequest* GetJenkinsRequest(QObject* parent = 0);
    SETTING_TYPE GetSettingType(QString type);
    void AbortJob_(QString job_name, int build_number);
    void GetNodes_(QString groovy_script);
    void ValidateSettings_();
    void jenkinsRequestFinished(JenkinsRequest* request);
    void storeJobConfiguration(QString job_name, QJsonDocument json);
    
    QJsonDocument getJobConfiguration(QString job_name);
    

    //Mutex used by all getters/setters
    QMutex mutex_;

    //Instance Variables
    QString url_;
    QString username_;
    QString token_;
    QString scipts_path_;
    QString job_name_;

    bool settings_validated_ = false;
    bool requesting_nodes_ = false;
    bool validating_settings = false;
    bool got_java_ = false;

    bool settings_changed = false;

    QFuture<void> validation_thread;
    QFuture<void> get_nodes_thread;


    //A Hash lookup of Jenkins Jobs JSON Documents
    QHash<QString, QJsonDocument> jobsJSON;
    //Vector of all Active JenkinsRequest objects created from this.
    QList<JenkinsRequest*> requests;

    ViewController* view_controller_ = 0;
};

#endif // JENKINSMANAGER_H
