#ifndef JENKINSREQUEST_H
#define JENKINSREQUEST_H
#include <QObject>
#include <QString>
#include <QNetworkAccessManager>

enum JOB_STATE{
    NO_JOB = 0,
    BUILDING,
    BUILT,
    FAILED
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

class JenkinsManager;

class JenkinsRequest: public QObject{
    Q_OBJECT
public:
    //Constructor
    JenkinsRequest(JenkinsManager* manager);
    ~JenkinsRequest();

signals:
    //Emitted once by the slot getJobParameters
    void gotJobParameters(QString jobName, Jenkins_JobParameters parameters);
    //Emitted once by the slot getJobActiveConfigurations
    void gotJobActiveConfigurations(QString jobName, QStringList configurations);
    //Emitted, one or more times, by the slot getJobConsoleOutput
    void gotLiveCLIOutput(QString jobName, int buildNumber, QString activeConfiguration, QString outputChunk);
    //Emitted once by the slot getJobConsoleOutput
    void gotJobConsoleOutput(QString jobName, int buildNumber, QString activeConfiguration, QString consoleOutput);
    //Emitted, one or more times, by the slot getJobState
    void gotJobStateChange(QString jobName, int buildNumber, QString activeConfiguration, JOB_STATE jobState);

    //Emitted, by any slot which deems the request to have failed.
    void requestFailed();
    //Emitted, by ALL slots once a slot has finished it's function.
    void requestFinished();

    void gotGroovyScriptOutput(QString consoleOutput);


private slots:
    //Slots are listed as private so they cannot be called directly, only connected too. All of these Slots have a matching return signal.
    void getJobActiveConfigurations(QString jobName);
    void getJobConsoleOutput(QString jobName, int buildNumber, QString activeConfiguration="");
    void getJobParameters(QString jobName);
    void getJobState(QString jobName, int buildNumber, QString activeConfiguration="");

    void runGroovyScript(QString groovyScriptPath);



    //Requests
    void buildJob(QString jobName, Jenkins_JobParameters jobParameters);
private:
    //CLI and HTTP getters
    QByteArray wget(QString url);
    QByteArray runProcess(QString command);

    //SLOT helper methods
    bool _isJobAMatrixProject(QString jobName);
    QStringList _getJobActiveConfigurations(QString jobName);
    JOB_STATE _getJobState(QString jobName, int buildNumber, QString activeConfiguration="");
    void storeRequestParameters(QString jobName, int buildNumber = -1, QString activeConfiguration = "");
    QJsonDocument getJobConfiguration(QString jobName, int buildNumber=-1, QString activeConfiguration="", bool reRequest=false);

    //Instance variables
    QString jobName;
    QString activeConfiguration;
    int buildNumber;
    int timeOutMS;

    //The parent JenkinsManager
    JenkinsManager* manager;
    //The Network Manager for request URLS
    QList<QNetworkAccessManager*> managers;
};

#endif // JENKINSREQUEST_H
