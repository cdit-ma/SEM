#ifndef JENKINSREQUEST_H
#define JENKINSREQUEST_H
#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcessEnvironment>

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

class JenkinsManager;
class JenkinsRequest: public QObject{
    friend class JenkinsManager;
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
    //Emitted, Once by the slot validateJenkinsSettings
    void gotSettingsValidationResponse(bool settingsValid, QString responseMessage);

    //Emitted, Once by the slot runGroovyScript
    void gotGroovyScriptOutput(QString consoleOutput);

    //Emitted, by any slot which deems the request to have failed.
    void requestFailed(QString data="");
    //Emitted, by ALL slots once a slot has finished it's function.
    void requestFinished();
    //Emitted, if the JenkinsManager is destroyed.
    void unexpectedTermination();

public slots:
    //Slots are listed as private so they cannot be called directly, only connected too. All of these Slots have a matching return signal.
    void getJobActiveConfigurations(QString jobName);
    void getJobConsoleOutput(QString jobName, int buildNumber, QString activeConfiguration="");
    void getJobParameters(QString jobName);
    void getJobState(QString jobName, int buildNumber, QString activeConfiguration="");

    //Requests
    void runGroovyScript(QString groovyScriptPath, QString parameters="");
    void buildJob(QString jobName, Jenkins_JobParameters jobParameters);
    void stopJob(QString jobName, int buildNumber, QString activeConfiguration="");
    void validateJenkinsSettings();

    void waitForJobNumber(QString jobName, int buildNumber, QString activeConfiguration, QString outputChunk);

    //Called if the JenkinsManager has been destroyed.
    void _unexpectedTermination();
private:
    QString validate();

    //CLI and HTTP getters
    QPair<int, QByteArray> wget(QString url, bool auth = true);
    QPair<int, QByteArray> post(QString url, QByteArray data = QByteArray());
    QPair<int, QByteArray> waitForReply(QNetworkReply* reply);
    QPair<int, QByteArray> runProcess(QString command, QProcessEnvironment environment=QProcessEnvironment());
    QPair<int, QByteArray> runProcess2(QString command, QStringList parameters);

    QNetworkAccessManager* getNetworkManager();

    //SLOT helper methods
    bool waitForValidSettings();
    bool _isJobAMatrixProject(QString jobName);
    QStringList _getJobActiveConfigurations(QString jobName);
    JOB_STATE _getJobState(QString jobName, int buildNumber, QString activeConfiguration="");
    void storeRequestParameters(QString jobName, int buildNumber = -1, QString activeConfiguration = "");
    QJsonDocument getJobConfiguration(QString jobName, int buildNumber=-1, QString activeConfiguration="", bool reRequest=false);

    //Instance variables
    QString jobName;
    QString currentOutput;
    QString activeConfiguration;
    int buildNumber;
    int timeOutMS;
    bool terminated;
    bool waitingOnNumber;

    //The parent JenkinsManager
    JenkinsManager* manager;
    //The Network Manager for request URLS
    QNetworkAccessManager* managers;
    //QList<QNetworkAccessManager*> managers;
};

#endif // JENKINSREQUEST_H
