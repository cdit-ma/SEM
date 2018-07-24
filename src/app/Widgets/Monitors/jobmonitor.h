#ifndef JOB_MONITOR_H
#define JOB_MONITOR_H

#include <QFrame>
#include <QStackedWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QToolBar>
#include <QLabel>
#include <QJsonDocument>

#include "monitor.h"

#include "../../Widgets/optiongroupbox.h"
#include "../../Controllers/JenkinsManager/jenkinsmanager.h"

struct Jenkins_Job_Status;
class JenkinsMonitor;
class ConsoleMonitor;
class Theme;
class JobMonitor: public QFrame
{
public:
    JobMonitor(JenkinsManager* jenkins_manager, QWidget *parent = 0);
    JenkinsMonitor* constructJenkinsMonitor(QString job_name, int build_number);
    ConsoleMonitor* constructConsoleMonitor();
    QToolBar* getToolbar();

    Monitor* getMonitor(QString job_name, int build_number);
    ConsoleMonitor* getConsoleMonitor(QString name);
    void refreshRecentBuildsByName(QString job_name);
private:
    void JobStatusChanged(Jenkins_Job_Status status);
    void MonitorStateChanged(Notification::Severity state);

    void themeChanged();
    void setupLayout();
    
    void refreshRecentBuilds();
    
    void gotJobConfig(QJsonDocument document);

    

    void MonitorClose();

    void updateMonitorIcon(Monitor* monitor, Theme* theme = 0);
    void updateMonitorIcon(QString job_name, int job_number, Notification::Severity state);

    void jobPressed();

    void stackedWidgetChanged(int index);
    void stackedWidgetRemoved(int index);

    void requestJobConsoleOutput(QString job_name, int job_id);

private:
    QPair<QString, int> splitJobKey(QString key);
    
    QString getJobKey(QString job_name, int job_number);

    JenkinsManager* jenkins_manager = 0;
    QHash<QString, Monitor*> monitors;
    QStackedWidget* stacked_widget = 0;

    QToolBar* toolbar = 0;
    
    QSplitter* splitter = 0;

    QWidget* jobs_list_widget = 0;
    QVBoxLayout* jobs_list_layout = 0;
    QScrollArea* jobs_list_scroll = 0;

    OptionGroupBox* running_jobs_box = 0;
};

#endif // JOB_MONITOR_H
