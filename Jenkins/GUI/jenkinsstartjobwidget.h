#ifndef JENKINSSTARTJOBWIDGET_H
#define JENKINSSTARTJOBWIDGET_H
#include "../jenkinsmanager.h"

#include "jenkinsloadingwidget.h"
#include "../../GUI/keyeditwidget.h"

#include <QDialog>
#include <QGroupBox>


class JenkinsStartJobWidget: public QDialog
{
    Q_OBJECT
public:
    JenkinsStartJobWidget(QWidget *parent = 0, JenkinsManager* jenkins=0);
    ~JenkinsStartJobWidget();

    void getJenkinsData(QString jobName);

    void requestJob(QString jobName, QString graphmlFile="");

signals:
    //Connected to JenkinsRequest SLOTS
    void getJobParameters(QString jobName);
    void buildJob(QString jobName, Jenkins_JobParameters parameters);

public slots:
    //Connected to from JenkinsRequest SIGNALS
    void gotJobParameters(QString jobName, Jenkins_JobParameters parameters);
    void authenticationFinished(bool success, QString message);
    //Connected to the build button in the GUI
    void build();
private:
    void setupLayout(QString jobName);

    KeyEditWidget* getParameterWidget(Jenkins_Job_Parameter parameter);

    //Represents the icon/title/loading Jenkins Bar
    JenkinsLoadingWidget* loadingWidget;

    //Top Section is contained in a titleWidget.
    QWidget* titleWidget;
    QGroupBox* parameterWidget;
    QWidget* buttonWidget;


    //The list of Parameter Widgets.
    QList<KeyEditWidget*> parameterWidgets;

    QString tempGraphMLFile;

    QString jobName;
    JenkinsManager* jenkins;
};



#endif // JENKINSSTARTJOBWIDGET_H
