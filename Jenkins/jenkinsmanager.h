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

class JenkinsManager: public QObject
{
    //Define Jenkins Request as friend to enable use of private methods
    friend class JenkinsRequest;

    Q_OBJECT
public:
    JenkinsManager(QString cliPath, QString url, QString username, QString password);
    bool hasValidSettings();
    JenkinsRequest* getJenkinsRequest(QObject* parent = 0, bool deleteOnCompletion = true);

private:
    void storeJobConfiguration(QString jobName, QJsonDocument json);
    QJsonDocument getJobConfiguration(QString jobName);

    void jenkinsRequestFinished(JenkinsRequest* request);


    //CLI getter helper functions.
    QString getURL();
    QString getCLIPrefix();
    QString getCLILoginSuffix();
    QString getCLIPath();
    QString getCLICommand(QString cliCommand);

    //Instance Variables
    QString url;
    QString username;
    QString password;
    QString cliPath;

    //A Hash lookup of Jenkins Jobs JSON Documents
    QHash<QString, QJsonDocument> jobsJSON;
    //Vector of all Active JenkinsRequest objects created from this.
    QList<JenkinsRequest*> requests;
};

#endif // JENKINSMANAGER_H
