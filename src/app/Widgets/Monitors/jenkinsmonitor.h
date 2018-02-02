#ifndef JENKINS_MONITOR_H
#define JENKINS_MONITOR_H

#include "monitor.h"
#include "../../Widgets/optiongroupbox.h"
#include <QTextBrowser>
#include <QToolBar>
#include <QLabel>

class JenkinsMonitor : public Monitor
{
    Q_OBJECT
public:
    JenkinsMonitor(QString job_name, int build_number, QWidget * parent = 0);
    QString GetJobName() const;
    int getBuildNumber() const;
signals:
    void GotoURL();
public slots:
    void gotJobArtifacts(QStringList artifacts);
private:
    void artifactPressed();
    void stateChanged(Notification::Severity state);
    void themeChanged();
    void setupLayout();
    void update_state_icon();

    QString job_name;
    int build_number;

    QLabel* icon_label = 0;
    QLabel* text_label = 0;
    QAction* abort_action = 0;
    QAction* clear_action = 0;
    QAction* close_action = 0;
    QAction* url_action = 0;
    QToolBar* toolbar = 0;

    OptionGroupBox* artifacts_box = 0;

    QTextBrowser* text_browser = 0;
};

#endif // JENKINS_MONITOR_H
