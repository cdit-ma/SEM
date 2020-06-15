#ifndef JENKINS_MONITOR_H
#define JENKINS_MONITOR_H

#include "monitor.h"
#include "../../Widgets/optiongroupbox.h"
#include "../../Controllers/JenkinsManager/jenkinsmanager.h"

#include <QTextBrowser>
#include <QToolBar>
#include <QLabel>
#include <QFutureWatcher>
#include <QTimer>

class JenkinsMonitor : public Monitor
{
    Q_OBJECT
    
public:
    explicit JenkinsMonitor(JenkinsManager* jenkins_manager, QString job_name, int build_number, QWidget* parent = nullptr);

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
    void SetUser(const QString& user);
    void SetDescription(const QString& description);

    void artifactPressed();
    void stateChanged(Notification::Severity state);
    void themeChanged();
    void setupLayout();
    void update_state_icon();

    QString job_name;
    int build_number;

    JenkinsManager* jenkins_manager = nullptr;
    QLabel* icon_label = nullptr;
    QLabel* text_label = nullptr;

    QLabel* duration_icon_label = nullptr;
    QLabel* duration_label = nullptr;

    QLabel* user_icon_label = nullptr;
    QLabel* user_label = nullptr;

    QLabel* description_icon_label = nullptr;
    QLabel* description_label = nullptr;

    QAction* abort_action = nullptr;
    QAction* close_action = nullptr;
    QAction* url_action = nullptr;

    QToolBar* top_toolbar = nullptr;
    QToolBar* bottom_toolbar = nullptr;

    OptionGroupBox* artifacts_box = nullptr;

    QFutureWatcher<QString>* console_watcher = nullptr;
    QFutureWatcher<Jenkins_Job_Status>* status_watcher = nullptr;
    QFutureWatcher<QJsonDocument>* artifact_watcher = nullptr;

    QTextBrowser* text_browser = nullptr;
    QTimer* refresh_timer = nullptr;
};

#endif // JENKINS_MONITOR_H