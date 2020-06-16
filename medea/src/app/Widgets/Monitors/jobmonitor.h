#ifndef JOB_MONITOR_H
#define JOB_MONITOR_H

#include "monitor.h"
#include "../../Widgets/optiongroupbox.h"
#include "../../Controllers/JenkinsManager/jenkinsmanager.h"

#include <QFrame>
#include <QStackedWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QToolBar>
#include <QLabel>
#include <QJsonDocument>

struct Jenkins_Job_Status;

class JenkinsMonitor;
class ConsoleMonitor;
class Theme;

class JobMonitor: public QFrame
{
    
public:
    explicit JobMonitor(JenkinsManager* jenkins_manager, QWidget* parent = nullptr);
    
    JenkinsMonitor* constructJenkinsMonitor(const QString& job_name, int build_number);
    ConsoleMonitor* constructConsoleMonitor();
    
    QToolBar* getToolbar();
    Monitor* getMonitor(const QString& job_name, int build_number);
    ConsoleMonitor* getConsoleMonitor(const QString& name);
    
    void refreshRecentBuildsByName(const QString& job_name);
    
private:
    void JobStatusChanged(const Jenkins_Job_Status& status);
    void MonitorStateChanged(Notification::Severity state);

    void themeChanged();
    void setupLayout();
    
    void refreshRecentBuilds();
    
    void gotJobConfig(const QJsonDocument& document);

    void MonitorClose();
    void jobPressed();

    void updateMonitorIcon(Monitor* monitor, Theme* theme = nullptr);
    void updateMonitorIcon(const QString& job_name, int job_number, Notification::Severity state);

    void requestJobConsoleOutput(const QString& job_name, int job_id);

    void stackedWidgetChanged(int index);

    QString getJobKey(const QString& job_name, int job_number);
    QPair<QString, int> splitJobKey(const QString& key);

    JenkinsManager* jenkins_manager = nullptr;
    QHash<QString, Monitor*> monitors;
    QStackedWidget* stacked_widget = nullptr;

    QToolBar* toolbar = nullptr;
    QSplitter* splitter = nullptr;

    QWidget* jobs_list_widget = nullptr;
    QVBoxLayout* jobs_list_layout = nullptr;
    QScrollArea* jobs_list_scroll = nullptr;

    OptionGroupBox* running_jobs_box = nullptr;
};

#endif // JOB_MONITOR_H