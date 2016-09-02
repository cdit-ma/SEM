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

#include "../../Controller/settingscontroller.h"

class JenkinsManager: public QObject
{
    //Define Jenkins Request as friend to enable use of private methods
    friend class JenkinsRequest;

    Q_OBJECT
public:
    JenkinsManager(QObject *parent);
    QString getUsername();

    void setURL(QString url);
    void setUsername(QString username);
    void setPassword(QString password);
    void setToken(QString token);
    void setJobName(QString jobname);

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
    void settingsValidationComplete(bool valid, QString message);

    void _runGroovyScript(QString, QString);
private slots:
    void _gotJenkinsNodes(QString data = "");
    void settingChanged(SETTING_KEY key, QVariant value);
    void gotSettingsValidationResponse(bool valid, QString message);
private:
    void setJenkinsBusy(bool busy);
    void validateJenkinsSettings();
    void storeJobConfiguration(QString jobName, QJsonDocument json);
    void clearJobConfigurations();
    QNetworkRequest getAuthenticatedRequest(QString url, bool auth =true);
    QJsonDocument getJobConfiguration(QString jobName);

    void jenkinsRequestFinished(JenkinsRequest* request);


    //CLI getter helper functions.
    QString getURL();
    QString getCLIPrefix();
    QString getCLILoginSuffix();
    QString getCLIPath();
    QString getCLICommand(QString cliCommand);
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
    bool urlChanged;
    //A Hash lookup of Jenkins Jobs JSON Documents
    QHash<QString, QJsonDocument> jobsJSON;
    //Vector of all Active JenkinsRequest objects created from this.
    QList<JenkinsRequest*> requests;
};

#endif // JENKINSMANAGER_H
