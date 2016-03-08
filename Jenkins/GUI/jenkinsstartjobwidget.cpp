#include "jenkinsstartjobwidget.h"
#include "../jenkinsmanager.h"
#include "jenkinsjobmonitorwidget.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QDialog>
#include <QPushButton>
#include <QProgressBar>
#include <QAction>
#include <QMenu>
#include <QProgressDialog>

/**
 * @brief JenkinsStartJobWidget::JenkinsStartJobWidget Constructor for the Jenkins Start Job Widget.
 * @param parent The parent QWidget
 * @param jenkins The JenkinsManager used to construct JenkinsRequest objects.
 */
JenkinsStartJobWidget::JenkinsStartJobWidget(QWidget *parent, JenkinsManager *jenkins):QDialog(parent)
{
    //Set the Jenkins Manager
    this->jenkins = jenkins;

    loadingWidget = 0;

    setWindowTitle("Launch Jenkins Job");
    setWindowIcon(QIcon(":/Actions/Job_Build.png"));

    setStyleSheet("font-family: Helvetica, Arial, sans-serif; background-color:white;  font-size: 13px; color: #333;");


    //Turn off the Other Buttons.
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));

    connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
    setModal(true);
}

JenkinsStartJobWidget::~JenkinsStartJobWidget()
{
   //No need to do anything.
}

/**
 * @brief JenkinsStartJobWidget::getJenkinsData Gets the Jenkins Data needed to fill this GUI
 * @param jobName The name of the Jenkins Job
 */
void JenkinsStartJobWidget::getJenkinsData(QString jobName)
{
    //Construct a new JenkinsRequest Object.
    JenkinsRequest* jenkinsP = jenkins->getJenkinsRequest(this);

    //Connect the emit signals from this Thread to the JenkinsRequest Thread.
    connect(this, SIGNAL(getJobParameters(QString)), jenkinsP, SLOT(getJobParameters(QString)));
    connect(jenkinsP, SIGNAL(gotJobParameters(QString,Jenkins_JobParameters)), this, SLOT(gotJobParameters(QString, Jenkins_JobParameters)));

    //Emit request SIGNAL
    getJobParameters(jobName);
    //Disconnect request SIGNAL.
    disconnect(this, SIGNAL(getJobParameters(QString)), jenkinsP, SLOT(getJobParameters(QString)));
}

/**
 * @brief JenkinsStartJobWidget::requestJob Requests the Jenkins Job Parameters and shows the GUI.
 * @param jobName The name of the Jenkins Job to Execute.
 * @param graphmlFile The graphmlFile to pass as a parameter if the Jenkins job requires one.
 */
void JenkinsStartJobWidget::requestJob(QString jobName, QString graphmlFile)
{
    tempGraphMLFile = graphmlFile;
    this->jobName = jobName;

    setupLayout(jobName);

    if(jenkins->hasValidatedSettings()){
        loadingWidget->setWaiting(true);
    }else{
        connect(jenkins, SIGNAL(settingsValidationComplete()), loadingWidget, SLOT(authenticationFinished()));
        connect(jenkins, SIGNAL(gotInvalidSettings(QString)), this, SLOT(reject()));
    }


    show();

    getJenkinsData(jobName);
}

/**
 * @brief JenkinsStartJobWidget::build Run when the Build button is pressed. Requests a Job to be built with the selected parameters. Shows a JenkinsJobMonitorWidget.
 */
void JenkinsStartJobWidget::build()
{
    Jenkins_JobParameters buildParameters;

    //Get the values from each of the Parameter Widgets.
    foreach(KeyEditWidget* parameterWidget, parameterWidgets){
        Jenkins_Job_Parameter parameter;
        if(parameterWidget){
            parameter.name = parameterWidget->getKeyName();
            parameter.value = parameterWidget->getValue().toString();
            buildParameters.append(parameter);
        }
    }



    //Construct a new JenkinsJobMonitorWidget to show the resulting Job build status. Connect it to the parent of this.
    JenkinsJobMonitorWidget* jjmw = new JenkinsJobMonitorWidget(this->parentWidget(), jenkins, jobName);

    //Get a new Jenkins Request Object. Tied to the new Jenkins Widget
    JenkinsRequest* jenkinsB = jenkins->getJenkinsRequest(jjmw);

    //Connect the buildJob SIGNAL to the Jenkins Request
    connect(this, SIGNAL(buildJob(QString,Jenkins_JobParameters)), jenkinsB, SLOT(buildJob(QString,Jenkins_JobParameters)));
    //Connect the gotJobStateChange SIGNAL to the JenkinsJobMonitorWidget so we can update the new GUI as the job changes.
    connect(jenkinsB, SIGNAL(gotJobStateChange(QString,int,QString,JOB_STATE)), jjmw, SLOT(jobStateChanged(QString,int,QString,JOB_STATE)));
    //Show the new Widget.
    jjmw->show();

    //Start the build
    buildJob(jobName, buildParameters);

    //Request a delete.
    deleteLater();
}

/**
 * @brief JenkinsStartJobWidget::gotJobParameters Connected to the JenkinsRequest::gotJobParameters() SIGNAL to provide this Widget with the Job Parameters.
 * @param jobName The name of the job the Parameters are for.
 * @param params The list of Jenkins Job Parameters
 */
void JenkinsStartJobWidget::gotJobParameters(QString, Jenkins_JobParameters params)
{
    //Hide the Loading Bar.
    loadingWidget->hideLoadingBar();

    //Set the visibility of the container widgets.
    titleWidget->setVisible(true);
    parameterWidget->setVisible(true);
    buttonWidget->setVisible(true);

    //Construct a group layout for the parameter widget.
    QVBoxLayout* groupVLayout = new QVBoxLayout();
    parameterWidget->setLayout(groupVLayout);

    //Construct a QWidget(KeyEditWidget) for each parameter.
    foreach(Jenkins_Job_Parameter parameter, params){
        KeyEditWidget* parameterEdit = getParameterWidget(parameter);

        //If we have got a non-null KeyEditWidget, we should add it to the Layout and list of Widgets.
        if(parameterEdit){
            groupVLayout->addWidget(parameterEdit);
            parameterWidgets.append(parameterEdit);
        }
    }
}


/**
 * @brief JenkinsStartJobWidget::setupLayout Sets up the Layout for the Widget.
 * @param jobName The Name of the Job.
 */
void JenkinsStartJobWidget::setupLayout(QString jobName)
{
    //Construct a Vertical Layout.
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);

    //Add a loading widget.
    loadingWidget = new JenkinsLoadingWidget();
    verticalLayout->addWidget(loadingWidget);

    //Setup Title.
    titleWidget = new QWidget();
    QHBoxLayout* titleLayout = new QHBoxLayout(titleWidget);
    verticalLayout->addWidget(titleWidget);

    //Set up a QLabel for the name of the Jenkins Job
    QLabel* jobLabel = new QLabel(jobName);
    QLabel* iconLabel = new QLabel();
    iconLabel->setPixmap(QPixmap::fromImage(QImage(":/Actions/Job_Build.png")));

    jobLabel->setStyleSheet("font-family: Helvetica, Arial, sans-serif; font-size: 18px;  font-weight: bold;");

    titleLayout->addWidget(iconLabel);
    titleLayout->addWidget(jobLabel);
    titleLayout->addStretch();
    //titleLayout->addWidget(jobLabel,0,Qt::AlignCenter);


    //Setup a QGroupBox for storing the Parameters
    parameterWidget = new QGroupBox();
    parameterWidget->setTitle("Job Parameters");
    verticalLayout->addWidget(parameterWidget, 1);

    //Construct a Button Widget.
    buttonWidget = new QWidget();


    QHBoxLayout * buttonLayout = new QHBoxLayout(buttonWidget);

    buttonLayout->setSpacing(0);
    buttonLayout->setMargin(0);

    //Add a Build Job Button
    QPushButton* buildButton = new QPushButton("Build");
    buttonLayout->addWidget(buildButton);


    //Connect the button to the build signal.
    connect(buildButton, SIGNAL(clicked()), this, SLOT(build()));

    //Set the Style to match the CSS from Jenkins
    buildButton->setStyleSheet("background-color: #4b758b; color: #eee; border: 1px solid #5788a1;font-weight: bold;  font-size: 12px;  font-family: Helvetica, Arial, sans-serif;   padding: 3px 20px;");

    //Add the Button widget to the layout.
    verticalLayout->addWidget(buttonWidget);

    //Setup the visiblity.
    titleWidget->setVisible(false);
    parameterWidget->setVisible(false);
    buttonWidget->setVisible(false);
}



/**
 * @brief JenkinsStartJobWidget::getParameterWidget Gets a KeyEditWidget for the appropriate type based on the Jenkins_Job_Parameter provided.
 * @param parameter The Jenkins Job Parameter
 * @return A KeyEditWidget
 */
KeyEditWidget* JenkinsStartJobWidget::getParameterWidget(Jenkins_Job_Parameter parameter)
{
    //Stores the type of the KeyEditWidget if it is custom.
    QString customType = "";
    //A Variable to store the type of Value
    QVariant valueType;

    if(parameter.type == "String"){
        //Set the Type to be String.
        valueType = QVariant(QVariant::String);
        valueType.setValue(parameter.defaultValue);
    }else if(parameter.type == "Boolean"){
        //Set the Type to be Boolean
        valueType = QVariant(QVariant::Bool);
        valueType.setValue(parameter.defaultValue=="true");
    }else if(parameter.type == "File"){
        //Set the Type/CustomType to be String/File
        customType = "File";
        valueType = QVariant(QVariant::String);
        //If we have been provided a tempGraphMLFile when this was constructed, use this.

        if(tempGraphMLFile != ""){
            valueType.setValue("\"" + tempGraphMLFile + "\"");
        }else{
            valueType.setValue(parameter.defaultValue);
        }
    }

    //Construct a new KeyEdit Widget with the provided parameters
    KeyEditWidget* keyEdit = new KeyEditWidget("",parameter.name, parameter.name, valueType, parameter.description, customType);
    return keyEdit;
}
