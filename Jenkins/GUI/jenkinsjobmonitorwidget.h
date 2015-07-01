#ifndef JENKINSJOBMONITORWIDGET_H
#define JENKINSJOBMONITORWIDGET_H

#include "../jenkinsmanager.h"
#include "../jenkinsrequest.h"
#include "jenkinsloadingwidget.h"
#include <QTextBrowser>
#include <QDialog>
#include <QLabel>
#include <QTabWidget>

//Forward Class Definition
class JenkinsManager;
class JenkinsJobMonitorWidget: public QDialog
{
    Q_OBJECT
public:
    JenkinsJobMonitorWidget(QWidget *parent = 0, JenkinsManager* jenkins=0, QString jobName="");
    ~JenkinsJobMonitorWidget();

private:
    void getJenkinsData();
    void setupLayout();


    void setJobState(QString activeConfiguration, JOB_STATE state);

signals:
    void getJobConsoleOutput(QString jobName, int buildNumber, QString activeConfiguration);
    void getJobActiveConfigurations(QString jobName);
    void getJobState(QString jobName, int buildNumber, QString activeConfiguration);
    void stopJob(QString jobName, int buildNumber, QString activeConfiguration);

public slots:
    void jobStateChanged(QString jobName, int buildNumber, QString activeConfiguration, JOB_STATE jobState);
    void gotJobActiveConfigurations(QString jobName, QStringList activeConfigurations);
    void gotJobConsoleOutput(QString jobName, int buildNumber, QString activeConfiguration, QString consoleOutput);

private slots:
    void stopPressed();
    void frameChanged(int frame);

private:

    QString htmlize(QString consoleOutput);
    QMovie* spinning;
    QString jobName;
    int buildNumber;

    bool buildingTabs[255];

    QLabel* jobIcon;
    QPushButton* stopButton;
    QLabel* jobLabel;

    QTabWidget* tabWidget;
    QWidget* titleWidget;
    JenkinsLoadingWidget* loadingWidget;

    QStringList configurations;
    bool requestedConsoleOutput;
    QHash<QString, QTextBrowser*> configurationBrowsers;
    JenkinsManager* jenkins;

};

#endif // JENKINSJOBMONITORWIDGET_H
