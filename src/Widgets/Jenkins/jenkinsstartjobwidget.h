#ifndef JENKINSSTARTJOBWIDGET_H
#define JENKINSSTARTJOBWIDGET_H

#include "../../Controllers/JenkinsManager/jenkinsrequest.h"

#include <QDialog>
#include <QGroupBox>
#include <QToolBar>
#include <QLabel>

class DataEditWidget;
class JenkinsManager;
class JenkinsStartJobWidget: public QDialog
{
    Q_OBJECT
public:
    JenkinsStartJobWidget(QString job_name, JenkinsManager* jenkins=0);
    ~JenkinsStartJobWidget();

    void getJenkinsData(QString jobName);

    void requestJob(QString jobName, QString graphmlFile="");

signals:
    //Connected to JenkinsRequest SLOTS
    void getJobParameters(QString jobName);
    void buildJob(QString jobName, Jenkins_JobParameters parameters);

public slots:
    void themeChanged();
    //Connected to from JenkinsRequest SIGNALS
    void gotJobParameters(QString jobName, Jenkins_JobParameters parameters);
    //Connected to the build button in the GUI
    void build();
private:
    void setupLayout();

    DataEditWidget* getParameterWidget(Jenkins_Job_Parameter parameter);

    QGroupBox* parameter_box;
    QList<DataEditWidget*> parameterWidgets;
    QToolBar* action_toolbar;
    QAction* build_action;
    QLabel* title_label;

    QString tempGraphMLFile;

    QString job_name;
    JenkinsManager* jenkins;
};



#endif // JENKINSSTARTJOBWIDGET_H
