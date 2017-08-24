#ifndef JENKINSREQUEST_H
#define JENKINSREQUEST_H

#include <QObject>
#include <QString>
#include <QJsonDocument>
#include <QMutex>
#include <QNetworkRequest>

enum JOB_STATE{
    NO_JOB = 0,
    BUILDING,
    BUILT,
    FAILED,
    ABORTED
};

struct Jenkins_Job_Parameter{
    //Used to execute.
    QString name;
    QString value;

    //Used for GUI
    QString type;
    QString description;
    QString defaultValue;
};

typedef QList<Jenkins_Job_Parameter> Jenkins_JobParameters;

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

    //Emitted once by the slot getJobConsoleOutput
    void GotJobConsoleOutput(QString job_name, int build_number, QString configuration, QString console_output);

    //Emitted, one or more times, by the slot getJobState
    void GotJobStateChange(QString job_name, int build_number, QString configuration, JOB_STATE jobState);

    //Emitted, Once by the slot ValidateSettings
    void GotValidatedSettings(bool valid, QString error_message);

    //Emitted, Once by the slot runGroovyScript
    void GotGroovyScriptOutput(bool success, QString output);

    //Emitted, Once by all slots when completed
    void Finished();

    //Called when the parent or the JenkinsManager is destroyed.
    void Terminate_();

public slots:
    void BuildJob(QString job_name, Jenkins_JobParameters jobParameters);
    void StopJob(QString job_name, int build_number, QString configuration="");

    void GetJobConsoleOutput(QString job_name, int build_number, QString configuration="");
    void GetJobParameters(QString job_name);

    void RunGroovyScript(QString groovy_script);
private slots:
    void ValidateSettings();

    //Called if the JenkinsManager has been destroyed.
    void Teardown_();
private:
    JOB_STATE getJobConsoleOutput(QString job_name, int build_number, QString configuration);
    QString getURL();
    QString getJobName();
    bool gotValidatedSettings();
    QNetworkRequest getAuthenticatedRequest(QString url, bool auth=true);

    ProcessRunner* GetRunner();
    QString combineJobURL(QString job_name, int build_number, QString configuration);

    bool BlockUntilValidatedSettings();

    JOB_STATE _getJobState(QString jobName, int buildNumber, QString activeConfiguration="");
    QJsonDocument _getJobConfiguration(QString jobName, int buildNumber=-1, QString activeConfiguration="", bool reRequest=false);


    QMutex mutex_;
    bool terminate_ = false;

    //The parent JenkinsManager
    JenkinsManager* jenkins_manager_ = 0;
    ProcessRunner* runner_ = 0;
};

#endif // JENKINSREQUEST_H
