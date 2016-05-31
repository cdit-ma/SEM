#ifndef JENKINSJOBMONITORWIDGET_H
#define JENKINSJOBMONITORWIDGET_H

#include "../jenkinsmanager.h"
#include "../jenkinsrequest.h"
#include "jenkinsloadingwidget.h"
#include <QTextBrowser>
#include <QDialog>
#include <QLabel>
#include <QTabWidget>
#include <QHBoxLayout>

//Forward Class Definition
class JenkinsManager;
class JenkinsJobMonitorWidget: public QDialog
{
    Q_OBJECT
public:
    JenkinsJobMonitorWidget(QWidget *parent, JenkinsManager* jenkins, QString jobName, Jenkins_JobParameters build_parameters);
    ~JenkinsJobMonitorWidget();

private:
    void getJenkinsData();
    void setupLayout();


    void setJobState(QString activeConfiguration, JOB_STATE state);

signals:
    void getJobConsoleOutput(QString jobName, int buildNumber, QString activeConfiguration);
    void getJobActiveConfigurations(QString jobName);
    void getJobState(QString jobName, int buildNumber, QString activeConfiguration);
    void cancelJob(QString jobName, int buildNumber, QString activeConfiguration);
    void stopJob(QString jobName, Jenkins_JobParameters parameters);

public slots:
    void jobStateChanged(QString jobName, int buildNumber, QString activeConfiguration, JOB_STATE jobState);
    void gotJobActiveConfigurations(QString jobName, QStringList activeConfigurations);
    void gotJobConsoleOutput(QString jobName, int buildNumber, QString activeConfiguration, QString consoleOutput);

private slots:
    void cancelPressed();
    void stopPressed();
    void frameChanged(int frame);
    void closeTab(int tabID);

    void authenticationFinished(bool success, QString message);

private:
    QString getBuildParameter(QString name);
    void haltRequested(QPushButton* button = 0);

    QString htmlize(QString consoleOutput);

    bool areTabsSpinning;
    QMovie* spinning;
    QString jobName;
    int buildNumber;

    bool buildingTabs[255];

    QHBoxLayout* titleLayout;
    QLabel* jobIcon;
    QPushButton* cancelButton;
    QPushButton* stopButton;
    QLabel* cancelLabel;
    QLabel* jobLabel;
    Jenkins_JobParameters buildParameters;
    QTabWidget* tabWidget;
    QWidget* titleWidget;
    JenkinsLoadingWidget* loadingWidget;

    QStringList configurations;
    bool requestedConsoleOutput;
    QHash<QString, QTextBrowser*> configurationBrowsers;
    JenkinsManager* jenkins;

};

#endif // JENKINSJOBMONITORWIDGET_H
