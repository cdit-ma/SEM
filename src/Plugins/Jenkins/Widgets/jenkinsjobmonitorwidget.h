#ifndef JENKINSJOBMONITORWIDGET_H
#define JENKINSJOBMONITORWIDGET_H

#include "../jenkinsmanager.h"
#include "../jenkinsrequest.h"
#include "jenkinsloadingwidget.h"
#include <QTextBrowser>
#include <QDialog>
#include <QLabel>
#include <QTabWidget>
#include <QToolBar>

//Forward Class Definition
class JenkinsManager;
class JenkinsJobMonitorWidget: public QDialog
{

    struct ConsoleMonitor{
        JOB_STATE state;
        QTextBrowser* browser;
    };

    Q_OBJECT
public:
    JenkinsJobMonitorWidget(QWidget *parent = 0, JenkinsManager* jenkins=0);
    ~JenkinsJobMonitorWidget();
signals:
    void getJobConsoleOutput(QString jobName, int buildNumber, QString activeConfiguration);
    void getJobState(QString jobName, int buildNumber, QString activeConfiguration);
    void stopJob(QString jobName, int buildNumber, QString activeConfiguration);

public slots:
    void gotJobStateChange(QString jobName, int buildNumber, QString activeConfiguration, JOB_STATE jobState);
    void gotJobConsoleOutput(QString jobName, int buildNumber, QString activeConfiguration, QString consoleOutput);
private slots:
    void themeChanged();
    void stopPressed();
    void frameChanged(int frame);
    void closeTab(int tabID);
private:
    void setupLayout();
    void setJobState(QString jobName, int buildNumber, QString config, JOB_STATE state);
    void setupTabs(QString job_name, int job_build, QStringList configurations);
    int getTabIndex(QString tab_name);
    QString getTabName(QString configuration, int build_number = -1);

    QString htmlize(QString consoleOutput);
    QMovie* movie_spinning;

    QString job_name;
    int job_build;

    QTabWidget* tabWidget;
    QLabel* job_label;
    QLabel* job_icon;

    QHash<QString, ConsoleMonitor*> consoleMonitors;

    JenkinsManager* jenkins;

    QAction* build_action;
    QAction* stop_action;
    QToolBar* action_toolbar;
};

#endif // JENKINSJOBMONITORWIDGET_H
