#ifndef JENKINSREQUEST_H
#define JENKINSREQUEST_H

#include <QObject>
#include <QString>
#include <QJsonDocument>
#include <QMutex>
#include <QNetworkRequest>
#include "../NotificationManager/notificationenumerations.h"
#include "../SettingsController/settingscontroller.h"

struct Jenkins_Job_Parameter{
    //Used to execute.
    QString name;
    QString value;

    //Used for GUI
    SETTING_TYPE type;
    QString description;
    QString defaultValue;
};

struct Jenkins_Job_Status{
    QString name;
    int number;
    Notification::Severity state;
    QString description;
};


typedef QList<Jenkins_Job_Parameter> Jenkins_JobParameters;
typedef QList<Jenkins_Job_Status> Jenkins_Job_Statuses;

class ProcessRunner;
class JenkinsManager;

class JenkinsRequest: public QObject{
    friend class JenkinsManager;
    Q_OBJECT

protected:
    //Constructor
    JenkinsRequest(JenkinsManager* jenkins_manager, QObject* parent = 0);
    ~JenkinsRequest();

signals:
    //Emitted once by the slot getJobParameters
    void GotJobParameters(QString job_name, Jenkins_JobParameters parameters);

    //Emitted, multiple times, by the slot getJobConsoleOutput, and by extension buildJob
    void GotLiveJobConsoleOutput(QString job_name, int build_number, QString configuration, QString console_output_chunk);

    void GotJobArtifacts(QString job_name, int build_number, QString configuration, QStringList artifact_urls);

    //Emitted once by the slot getJobConsoleOutput
    void GotJobConsoleOutput(QString job_name, int build_number, QString configuration, QString console_output);

    //Emitted, one or more times, by the slot getJobState
    void GotJobStateChange(QString job_name, int build_number, QString configuration, Notification::Severity job_state);

    //Emitted, Once by the slot ValidateSettings
    void GotValidatedSettings(bool valid, QString error_message);

    //Emitted, Once by the slot runGroovyScript
    void GotGroovyScriptOutput(bool success, QString output);

    //Emitted, Once by the slot GetRecentJobs
    void gotRecentJobs(Jenkins_Job_Statuses recent_jobs);

    //Emitted Once;
    void BuildingJob(QString job_name);

    //Emitted, Once by all slots when completed
    void Finished();

    //Called when the parent or the JenkinsManager is destroyed.
    void Terminate_();

public slots:
    void BuildJob(QString job_name, Jenkins_JobParameters jobParameters);
    void StopJob(QString job_name, int build_number, QString configuration="");

    void GetRecentJobs(QString job_name, int max_request_count);

    void GetJobConsoleOutput(QString job_name, int build_number, QString configuration="");
    void GetJobParameters(QString job_name);

    void RunGroovyScript(QString groovy_script);
private slots:
    void ValidateSettings();

    //Called if the JenkinsManager has been destroyed.
    void Teardown_();
private:
    Notification::Severity getJobConsoleOutput(QString job_name, int build_number, QString configuration="");
    QString getURL();
    QString getJobName();
    bool gotValidatedSettings();
    QNetworkRequest getAuthenticatedRequest(QString url, bool auth=true);

    ProcessRunner* GetRunner();
    QString combineJobURL(QString job_name, int build_number, QString configuration);

    bool BlockUntilValidatedSettings();

    Jenkins_Job_Status _getJobState(QString jobName, int buildNumber, QString activeConfiguration="", bool re_request=false);
    QJsonDocument _getJobConfiguration(QString jobName, int buildNumber=-1, QString activeConfiguration="", bool reRequest=false);


    QMutex mutex_;
    bool terminate_ = false;

    //The parent JenkinsManager
    JenkinsManager* jenkins_manager_ = 0;
    ProcessRunner* runner_ = 0;
};

#endif // JENKINSREQUEST_H
