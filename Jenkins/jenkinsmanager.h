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

class JenkinsManager: public QObject
{
    //Define Jenkins Request as friend to enable use of private methods
    friend class JenkinsRequest;

    Q_OBJECT
public:
    JenkinsManager(QString cliBinaryPath);
    QString getUsername();

    void setURL(QString url);
    void setUsername(QString username);
    void setPassword(QString password);
    void setToken(QString token);
    void setJobName(QString jobname);

    bool hasSettings();
    bool hasValidatedSettings();
    JenkinsRequest* getJenkinsRequest(QObject* parent = 0, bool deleteOnCompletion = true);
    void validateSettings();
signals:
    void settingsValidationComplete();
    void tryValidateSettings();
    void settingsValidationComplete(bool valid, QString message);
private slots:
    void gotSettingsValidationResponse(bool valid, QString message);
private:
    void validateJenkinsSettings();
    void storeJobConfiguration(QString jobName, QJsonDocument json);
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

    bool settingsValidated;
    bool settingsValidating;
    //A Hash lookup of Jenkins Jobs JSON Documents
    QHash<QString, QJsonDocument> jobsJSON;
    //Vector of all Active JenkinsRequest objects created from this.
    QList<JenkinsRequest*> requests;
};

#endif // JENKINSMANAGER_H
