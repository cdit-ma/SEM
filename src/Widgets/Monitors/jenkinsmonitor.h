#ifndef JENKINS_MONITOR_H
#define JENKINS_MONITOR_H

#include "monitor.h"
#include <QTextBrowser>
#include <QToolBar>

class JenkinsMonitor : public Monitor
{
    Q_OBJECT
public:
    JenkinsMonitor(QString job_name, int build_number, QWidget * parent = 0);
    QString GetJobName() const;
    int getBuildNumber() const;
signals:
    void GotoURL();
private:
    void stateChanged(Notification::Severity state);
    void themeChanged();
    void setupLayout();

    QString job_name;
    int build_number;

    QAction* abort_action = 0;
    QAction* clear_action = 0;
    QAction* url_action = 0;
    QToolBar* toolbar = 0;

    QTextBrowser* text_browser = 0;
};

#endif // JENKINS_MONITOR_H
