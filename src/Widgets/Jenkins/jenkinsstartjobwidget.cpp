#include "jenkinsstartjobwidget.h"
#include "jenkinsjobmonitorwidget.h"

#include "../../Controllers/JenkinsManager/jenkinsmanager.h"
#include "../../Controllers/JenkinsManager/jenkinsrequest.h"
#include "../../Controllers/ActionController/actioncontroller.h"
#include "../../Utils/filehandler.h"
#include "../dataeditwidget.h"
#include "../../theme.h"



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
#include <QStringBuilder>

/**
 * @brief JenkinsStartJobWidget::JenkinsStartJobWidget Constructor for the Jenkins Start Job Widget.
 * @param parent The parent QWidget
 * @param jenkins The JenkinsManager used to construct JenkinsRequest objects.
 */
JenkinsStartJobWidget::JenkinsStartJobWidget(QString job_name, JenkinsManager *jenkins):QDialog()
{
    //Set the Jenkins Manager
    this->jenkins = jenkins;
    this->job_name = job_name;

    setWindowTitle("Jenkins Job:: " + job_name);
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));

    setupLayout();

    setModal(true);
    connect(Theme::theme(), &Theme::theme_Changed, this, &JenkinsStartJobWidget::themeChanged);
    themeChanged();

    connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

JenkinsStartJobWidget::~JenkinsStartJobWidget()
{
}

/**
 * @brief JenkinsStartJobWidget::getJenkinsData Gets the Jenkins Data needed to fill this GUI
 * @param jobName The name of the Jenkins Job
 */
void JenkinsStartJobWidget::getJenkinsData(QString jobName)
{
    //Construct a new JenkinsRequest Object.
    JenkinsRequest* jenkinsRequest = jenkins->GetJenkinsRequest(this);

    //Connect the emit signals from this Thread to the JenkinsRequest Thread.
    auto r = connect(this, &JenkinsStartJobWidget::getJobParameters, jenkinsRequest, &JenkinsRequest::GetJobParameters);
    connect(jenkinsRequest, &JenkinsRequest::GotJobParameters, this, &JenkinsStartJobWidget::gotJobParameters);

    //Emit request SIGNAL
    emit getJobParameters(jobName);
    disconnect(r);
}

/**
 * @brief JenkinsStartJobWidget::requestJob Requests the Jenkins Job Parameters and shows the GUI.
 * @param jobName The name of the Jenkins Job to Execute.
 * @param graphmlFile The graphmlFile to pass as a parameter if the Jenkins job requires one.
 */
void JenkinsStartJobWidget::requestJob(QString jobName, QString graphmlFile)
{
    tempGraphMLFile = graphmlFile;

    if(jenkins->GotValidSettings()){
        //loadingWidget->setWaiting(true);
    }else{
        connect(jenkins, SIGNAL(settingsValidationComplete(bool,QString)), this, SLOT(authenticationFinished(bool, QString)));
    }


    show();

    getJenkinsData(jobName);
}

void JenkinsStartJobWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getWidgetStyleSheet("JenkinsStartJobWidget") % theme->getGroupBoxStyleSheet() % theme->getScrollBarStyleSheet() % theme->getLabelStyleSheet());
    action_toolbar->setStyleSheet(theme->getToolBarStyleSheet());
    title_label->setStyleSheet(theme->getTitleLabelStyleSheet());

    build_action->setIcon(theme->getIcon("Icons", "jobBuild"));
    setWindowIcon(theme->getIcon("Icons", "jenkins"));
}

/**
 * @brief JenkinsStartJobWidget::build Run when the Build button is pressed. Requests a Job to be built with the selected parameters. Shows a JenkinsJobMonitorWidget.
 */
void JenkinsStartJobWidget::build()
{
    Jenkins_JobParameters buildParameters;

    //Get the values from each of the Parameter Widgets.
    foreach(DataEditWidget* parameterWidget, parameterWidgets){
        Jenkins_Job_Parameter parameter;
        if(parameterWidget){
            parameter.name = parameterWidget->getLabel();
            parameter.value = parameterWidget->getValue().toString();
            buildParameters.append(parameter);
        }
    }

    //Get a new Jenkins Request Object. Tied to the new Jenkins Widget
    auto jjmw = jenkins->GetJobMonitorWidget();
    JenkinsRequest* jenkins_build = jenkins->GetJenkinsRequest(jjmw);

    connect(this, &JenkinsStartJobWidget::buildJob, jenkins_build, &JenkinsRequest::BuildJob);
    connect(jenkins_build, &JenkinsRequest::GotJobStateChange, jjmw, &JenkinsJobMonitorWidget::gotJobStateChange);
    connect(jenkins_build, &JenkinsRequest::GotLiveJobConsoleOutput, jjmw, &JenkinsJobMonitorWidget::gotJobConsoleOutput);

    //Show the new Widget.
    jjmw->show();

    //Start the build
    emit buildJob(job_name, buildParameters);

    disconnect(this, &JenkinsStartJobWidget::buildJob, jenkins_build, &JenkinsRequest::BuildJob);

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
    //Construct a group layout for the parameter widget.
    QVBoxLayout* parameter_layout = new QVBoxLayout();
    parameter_box->setLayout(parameter_layout);

    //If model


    //Construct a QWidget(KeyEditWidget) for each parameter.
    foreach(Jenkins_Job_Parameter parameter, params){
        auto parameterEdit = getParameterWidget(parameter);
        if(parameter.name == "model"){
            //RE_GEN takes the stringd contents
            auto fileContents = FileHandler::readTextFile(tempGraphMLFile);
            parameterEdit->setValue(fileContents);
            parameterEdit->setEnabled(false);
        }else if(parameter.name == "model.graphml"){
            //MEDEA SEM takes a file pointer
            QString model_path = tempGraphMLFile;
            parameterEdit->setValue(model_path);
            parameterEdit->setEnabled(false);
        }

        //If we have got a non-null KeyEditWidget, we should add it to the Layout and list of Widgets.
        if(parameterEdit){
            parameter_layout->addWidget(parameterEdit);
            parameterWidgets.append(parameterEdit);
        }
    }
    parameter_layout->addStretch();
}

/**
 * @brief JenkinsStartJobWidget::setupLayout Sets up the Layout for the Widget.
 * @param jobName The Name of the Job.
 */
void JenkinsStartJobWidget::setupLayout()
{
    //Construct a Vertical Layout.
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);

    //Add a loading widget.
    //loadingWidget = new JenkinsLoadingWidget();
    //verticalLayout->addWidget(loadingWidget);

    //Setup Title.
    QHBoxLayout* title_layout = new QHBoxLayout();
    verticalLayout->addLayout(title_layout);

    //Set up a QLabel for the name of the Jenkins Job
    title_label = new QLabel(job_name, this);
    QLabel* job_icon_label = new QLabel(this);
    job_icon_label->setPixmap(Theme::theme()->getImage("Icons", "jenkins", QSize(32,32)));

    title_layout->addWidget(job_icon_label);
    title_layout->addWidget(title_label);
    title_layout->addStretch();


    //Setup a QGroupBox for storing the Parameters
    parameter_box = new QGroupBox(this);
    parameter_box->setTitle("Job Parameters");
    verticalLayout->addWidget(parameter_box, 1);


    action_toolbar = new QToolBar(this);
    action_toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    build_action = action_toolbar->addAction("Build Job");
    verticalLayout->addWidget(action_toolbar, 0, Qt::AlignRight);

    //Connect the button to the build signal.
    connect(build_action, &QAction::triggered, this, &JenkinsStartJobWidget::build);
}



/**
 * @brief JenkinsStartJobWidget::getParameterWidget Gets a KeyEditWidget for the appropriate type based on the Jenkins_Job_Parameter provided.
 * @param parameter The Jenkins Job Parameter
 * @return A KeyEditWidget
 */
DataEditWidget *JenkinsStartJobWidget::getParameterWidget(Jenkins_Job_Parameter parameter)
{
    SETTING_TYPE type = SETTING_TYPE::STRING;

    if(parameter.type == "String"){
        //Set the Type to be String.
        type = SETTING_TYPE::STRING;
    }else if(parameter.type == "Boolean"){
        //Set the Type to be Boolean
        type = SETTING_TYPE::BOOL;
    }else if(parameter.type == "File"){
        type = SETTING_TYPE::FILE;
    }

    DataEditWidget* dataEdit = new DataEditWidget(parameter.name, type, parameter.defaultValue);
    ////Construct a new KeyEdit Widget with the provided parameters
    //KeyEditWidget* keyEdit = new KeyEditWidget("",parameter.name, parameter.name, valueType, parameter.description, customType);
    return dataEdit;
}
