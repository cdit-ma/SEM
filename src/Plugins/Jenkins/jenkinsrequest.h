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

struct HTTPResult{
    bool success = false;
    int error_code = 0;
    QString standard_output;
    QString location_header;
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

    //Emitted, one or more times, by the slot getJobConsoleOutput
    void gotLiveJobConsoleOutput(QString jobName, int buildNumber, QString activeConfiguration, QString outputChunk);

    //Emitted once by the slot getJobConsoleOutput
    void gotJobConsoleOutput(QString jobName, int buildNumber, QString activeConfiguration, QString consoleOutput);

    //Emitted, one or more times, by the slot getJobState
    void gotJobStateChange(QString jobName, int buildNumber, QString activeConfiguration, JOB_STATE jobState);

    //Emitted, Once by the slot validateJenkinsSettings
    void gotSettingsValidationResponse(bool settingsValid, QString responseMessage);

    //Emitted, Once by the slot runGroovyScript
    void gotGroovyScriptOutput(bool success, QString consoleOutput);

    //Emitted, by any slot which deems the request to have failed.
    void requestFailed(QString data="");
    //Emitted, by ALL slots once a slot has finished it's function.
    void requestFinished();
    //Emitted, if the JenkinsManager is destroyed.
    void unexpectedTermination();

public slots:
    void getJobConsoleOutput(QString job_name, int build_number, QString configuration="");
    void buildJob(QString job_name, Jenkins_JobParameters jobParameters);

    void getJobParameters(QString jobName);


    //Requests
    void runGroovyScript(QString groovy_script);
    void stopJob(QString jobName, int buildNumber, QString configuration="");

    void validateJenkinsSettings();

    //Called if the JenkinsManager has been destroyed.
    void _unexpectedTermination();
private:
    HTTPResult wget2(QNetworkRequest request);
    HTTPResult post2(QNetworkRequest request, QByteArray post_data = QByteArray());
    HTTPResult waitForNetworkReply(QNetworkReply* reply);

    QString combineJobURL(QString job_name, int build_number, QString configuration);
    QString validate();

    QNetworkAccessManager* getNetworkManager();

    //SLOT helper methods
    bool waitForValidSettings();
    JOB_STATE _getJobState(QString jobName, int buildNumber, QString activeConfiguration="");
    QJsonDocument _getJobConfiguration(QString jobName, int buildNumber=-1, QString activeConfiguration="", bool reRequest=false);

    //Instance variables
    int timeOutMS;
    bool terminated;

    //The parent JenkinsManager
    JenkinsManager* manager;
    //The Network Manager for request URLS
    QNetworkAccessManager* managers;
};

#endif // JENKINSREQUEST_H
