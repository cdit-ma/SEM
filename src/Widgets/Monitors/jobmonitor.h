#ifndef JOB_MONITOR_H
#define JOB_MONITOR_H

#include <QFrame>
#include <QTabWidget>
#include <QToolBar>

#include "monitor.h"
class JenkinsMonitor;
class ConsoleMonitor;
class Theme;
class JobMonitor: public QFrame
{
public:
    JobMonitor(QWidget *parent = 0);
    JenkinsMonitor* constructJenkinsMonitor(QString job_name, int build_number);
    ConsoleMonitor* constructConsoleMonitor(QString name);
    QToolBar* getToolbar();

    JenkinsMonitor* getJenkinsMonitor(QString job_name, int build_number);
    ConsoleMonitor* getConsoleMonitor(QString name);
private:
    void MonitorStateChanged(Notification::Severity state);
    void themeChanged();
    void frameChanged();
    void closeTab(int tabID);
    void setupLayout();

    void updateTabIcon(int tab_id, Theme* theme = 0);

    int getTab(QWidget* widget);
private:
    QHash<QString, Monitor*> monitors;
    QTabWidget* tab_widget = 0;
    QToolBar* toolbar = 0;
    QMovie* loading_movie = 0;
};

#endif // JOB_MONITOR_H
