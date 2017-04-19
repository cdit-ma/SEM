#ifndef JENKINSMANAGER_H
#define JENKINSMANAGER_H

//Include JenkinsRequest
#include "jenkinsrequest.h"

//Include QT Objects
#include <QObject>
#include <QJsonDocument>
#include <QString>
#include <QVector>
#include <QHash>
#include <QThread>
#include <QNetworkReply>
#include <QAuthenticator>

#include "../../Controllers/ActionController/actioncontroller.h"

class JenkinsJobMonitorWidget;

#include "../../Controllers/SettingsController/settingscontroller.h"

class JenkinsManager: public QObject
{
    //Define Jenkins Request as friend to enable use of private methods
    friend class JenkinsRequest;

    Q_OBJECT
public:
    JenkinsManager(QObject *parent);
    void setActionController(ActionController* actionController);
    ActionController* getActionController();
    QString getUsername();


    JenkinsJobMonitorWidget* getJobMonitorWidget();
    void setURL(QString url);
    void setUsername(QString username);
    void setPassword(QString password);
    void setToken(QString token);
    void setJobName(QString jobname);

    bool gotJobConfiguration(QString jobName);

    QJsonDocument getJobConfiguration(QString jobName);
    QStringList getActiveConfigurations(QString jobName);

    bool hasSettings();
    bool hasValidatedSettings();
    JenkinsRequest* getJenkinsRequest(QObject* parent = 0, bool deleteOnCompletion = true);
public slots:
    void validateSettings();
    void getJenkinsNodes();
    void executeJenkinsJob(QString modelFilePath);
signals:
    void gotJenkinsNodeGraphml(QString data);
    void jenkinsReady(bool ready);



    void tryValidateSettings();
    void checkForJava();
    void settingsValidationComplete(bool valid, QString message);
    void gotValidJava(bool valid, QString javaVersion);

    void _runGroovyScript(QString, QString);
private slots:
    void _gotJenkinsNodes(QString data = "");
    void settingChanged(SETTING_KEY key, QVariant value);
    void gotSettingsValidationResponse(bool valid, QString message);
private:

    void setJenkinsBusy(bool busy);
    void validateJenkinsSettings();
    void _checkForJava();
    void storeJobConfiguration(QString jobName, QJsonDocument json);
    void clearJobConfigurations();
    QNetworkRequest getAuthenticatedRequest(QString url, bool auth =true);


    void jenkinsRequestFinished(JenkinsRequest* request);

    ActionController* actionController;
    //CLI getter helper functions.
    QString getURL();
    QString getCLIPrefix();
    QString getCLILoginSuffix();
    QString getCLIPath();
    QString getCLICommand(QString cliCommand);
    QStringList getCLIArguments(QStringList args);
    QString getJobName();

    //Instance Variables
    QString url;
    QString username;
    QString password;
    QString token;
    QString cliBinaryPath;
    QString scriptPath;
    QString jobName;

    bool _jenkinsBusy;
    bool settingsValidated;
    bool _gotJava;
    bool urlChanged;
    //A Hash lookup of Jenkins Jobs JSON Documents
    QHash<QString, QJsonDocument> jobsJSON;
    //Vector of all Active JenkinsRequest objects created from this.
    QList<JenkinsRequest*> requests;

    JenkinsJobMonitorWidget* jenkinsJobGUI;
};

#endif // JENKINSMANAGER_H
