#ifndef JENKINSMANAGER_H
#define JENKINSMANAGER_H

#include "../SettingsController/settingscontroller.h"

#include <QObject>
#include <QJsonDocument>
#include <QString>
#include <QVector>
#include <QHash>
#include <QNetworkReply>
#include <QMutex>

//Forward Declare
class JenkinsRequest;
class ActionController;
class JenkinsJobMonitorWidget;

class JenkinsManager: public QObject
{
    //Define Jenkins Request as friend to enable use of private methods
    friend class JenkinsRequest;

    Q_OBJECT
public:
    JenkinsManager(QObject *parent);

    void setActionController(ActionController* action_controller);
    void SetUrl(QString url);
    void SetUser(QString user);
    void SetApiToken(QString api_token);
    void SetJobName(QString job_name);

    QString GetUrl();
    QString GetUser();
    QString GetJobName();
    QString GetApiToken();

    QStringList GetJobConfigurations(QString job_name);

    ActionController* GetActionController();
    JenkinsJobMonitorWidget* GetJobMonitorWidget();

    bool HasSettings();
    bool GotValidSettings();

    JenkinsRequest* GetJenkinsRequest(QObject* parent = 0);

    //Functions
    void ValidateSettings();
    void GetNodes();
    void BuildJob(QString model_file);
signals:
    //To GUI
    void GotJenkinsNodes(QString data);
    void JenkinsReady(bool ready);
    void gotValidSettings(bool valid, QString message);

    //To JenkinsRequests
    void ValidateSettings_();
    void RunGroovyScript_(QString script);
private slots:
    //From JenkinsRequests
    void GotJenkinsNodes_(bool success, QString data);
    void GotValidatedSettings_(bool valid, QString message);

    //From SettingsController
    void SettingChanged(SETTING_KEY key, QVariant value);
private:
    void jenkinsRequestFinished(JenkinsRequest* request);
    void storeJobConfiguration(QString job_name, QJsonDocument json);

    QJsonDocument getJobConfiguration(QString job_name);
    QNetworkRequest getAuthenticatedRequest(QString url, bool auth=true);

    //Mutex used by all getters/setters
    QMutex mutex_;

    //Instance Variables
    QString url_;
    QString username_;
    QString token_;
    QString scipts_path_;
    QString job_name_;

    bool settings_validating_ = false;
    bool settings_validated_ = false;
    bool requesting_nodes_ = false;
    bool got_java_ = false;

    //A Hash lookup of Jenkins Jobs JSON Documents
    QHash<QString, QJsonDocument> jobsJSON;
    //Vector of all Active JenkinsRequest objects created from this.
    QList<JenkinsRequest*> requests;

    ActionController* action_controller_ = 0;
    JenkinsJobMonitorWidget* job_monitor_widget_ = 0;
};

#endif // JENKINSMANAGER_H
