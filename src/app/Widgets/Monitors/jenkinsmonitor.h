#ifndef JENKINS_MONITOR_H
#define JENKINS_MONITOR_H

#include "monitor.h"
#include "../../Widgets/optiongroupbox.h"
#include <QTextBrowser>
#include <QToolBar>
#include <QLabel>
#include <QFutureWatcher>
#include <QTimer>
#include "../../Controllers/JenkinsManager/jenkinsmanager.h"

class JenkinsMonitor : public Monitor
{
    Q_OBJECT
public:
    JenkinsMonitor(JenkinsManager* jenkins_manager, QString job_name, int build_number, QWidget * parent = 0);
    ~JenkinsMonitor();
    QString GetJobName() const;
    int getBuildNumber() const;
signals:
    void GotoURL();
private:
    void ConsoleUpdated();
    void StatusUpdated();
    void Refresh();
    void JobFinished();

    void SetDuration(int duration);
    void SetUser(QString user);
    void SetDescription(QString description);

    void artifactPressed();
    void stateChanged(Notification::Severity state);
    void themeChanged();
    void setupLayout();
    void update_state_icon();
    void update_info();

    QString job_name;
    int build_number;

    JenkinsManager* jenkins_manager = 0;
    QLabel* icon_label = 0;
    QLabel* text_label = 0;

    QLabel* duration_icon_label = 0;
    QLabel* duration_label = 0;

    QLabel* user_icon_label = 0;
    QLabel* user_label = 0;

    QLabel* description_icon_label = 0;
    QLabel* description_label = 0;

    QAction* abort_action = 0;
    QAction* clear_action = 0;
    QAction* close_action = 0;
    QAction* url_action = 0;
    QToolBar* top_toolbar = 0;
    QToolBar* bottom_toolbar = 0;

    OptionGroupBox* artifacts_box = 0;

    QFutureWatcher<QString>* console_watcher = 0;
    QFutureWatcher<Jenkins_Job_Status>* status_watcher = 0;
    QFutureWatcher<QJsonDocument>* artifact_watcher = 0;
    
    
    QTextBrowser* text_browser = 0;
    QTimer* refresh_timer = 0;
};

#endif // JENKINS_MONITOR_H
