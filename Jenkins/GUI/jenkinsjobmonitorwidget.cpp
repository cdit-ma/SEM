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
#include <QMovie>

/**
 * @brief JenkinsJobMonitorWidget::JenkinsJobMonitorWidget A Widget which represents and displays the Output of a Jenkins Job which has just been invoked.
 * @param parent The Parent Widget for this Dialog
 * @param jenkins The Jenkins Manager used to spawn requests.
 * @param jobName The Name of the job started.
 */
JenkinsJobMonitorWidget::JenkinsJobMonitorWidget(QWidget *parent, JenkinsManager *jenkins, QString jobName):QDialog(parent)
{
    this->jenkins = jenkins;
    this->jobName = jobName;
    this->buildNumber = -1;
    requestedConsoleOutput = false;

    setWindowTitle("Jenkins Job Monitor");
    setWindowIcon(QIcon(":/Resources/Icons/jenkins_build.png"));

    setStyleSheet("font-family: Helvetica, Arial, sans-serif; background-color:white;  font-size: 13px; color: #333; ");

    setupLayout();

    //Request the JenkinsData
    getJenkinsData();

    //Turn off the Other Buttons.
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));

    connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

/**
 * @brief JenkinsJobMonitorWidget::~JenkinsJobMonitorWidget Destructor
 */
JenkinsJobMonitorWidget::~JenkinsJobMonitorWidget()
{
    //Do Nothing.
}

/**
 * @brief JenkinsJobMonitorWidget::getJenkinsData Gets the Jenkins Data needed to fill this GUI
 */
void JenkinsJobMonitorWidget::getJenkinsData()
{
    //Construct a new JenkinsRequest Object.
    JenkinsRequest* jenkinsRX = jenkins->getJenkinsRequest(this);

    //Connect the emit signals from this Thread to the JenkinsRequest Thread.
    connect(this, SIGNAL(getJobActiveConfigurations(QString)), jenkinsRX, SLOT(getJobActiveConfigurations(QString)));

    //Connect the gotJobActiveConfigurations SIGNAL to this so we can update the new GUI as the job changes.
    connect(jenkinsRX, SIGNAL(gotJobActiveConfigurations(QString,QStringList)), this, SLOT(gotJobActiveConfigurations(QString,QStringList)));

    //Request the Active Configurations for this job.
    getJobActiveConfigurations(jobName);
}

/**
 * @brief JenkinsJobMonitorWidget::setupLayout Sets up the Layout for the Widget.
 */
void JenkinsJobMonitorWidget::setupLayout()
{
    //Construct a Vertical Layout.
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);

    //Add A loading widget.
    loadingWidget = new JenkinsLoadingWidget();
    verticalLayout->addWidget(loadingWidget);


    //Setup Title.
    titleWidget = new QWidget();
    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleWidget->setLayout(titleLayout);

    //Set up a QLabel for the Building Icon
    jobIcon = new QLabel(":/Resources/Icons/jenkins_build.png");
    //Setup a QLabel for the Job Name
    jobLabel = new QLabel(jobName);
    jobLabel->setStyleSheet("font-family: Helvetica, Arial, sans-serif; font-size: 18px;  font-weight: bold;");

    titleLayout->addWidget(jobIcon);
    titleLayout->addWidget(jobLabel);
    titleLayout->addStretch();

    verticalLayout->addWidget(titleWidget);

    //Setup the Title
    tabWidget = new QTabWidget();
    verticalLayout->addWidget(tabWidget,1);

    //Hide the Title Widget and Tab Widget.
    titleWidget->hide();
    tabWidget->hide();
}

/**
 * @brief JenkinsJobMonitorWidget::setJobState Changes the Icon of the jobIcon to represent the current Job State.
 * @param activeConfiguration The name of the ActiveConfiguration the job state is for
 * @param state
 */
void JenkinsJobMonitorWidget::setJobState(QString activeConfiguration, JOB_STATE state)
{

    QString resourceName = "";


    if(state == BUILDING){
        if(activeConfiguration == ""){
            QMovie* movie = new QMovie(this);
            movie->setFileName(":/Resources/Icons/jenkins_building.gif");
            movie->start();
            jobIcon->setMovie(movie);
            jobLabel->setText(jobName + " Build #" + QString::number(buildNumber));
        }
    }else if(state == BUILT){
        resourceName = ":/Resources/Icons/jenkins_built.png";
    }else if(state == FAILED){
        resourceName = ":/Resources/Icons/jenkins_failed.png";
    }

    int index = configurations.indexOf(activeConfiguration);
    if(index >=0 && resourceName != ""){
        if(activeConfiguration == ""){
            jobIcon->setPixmap(QPixmap::fromImage(QImage(resourceName)));
        }
        tabWidget->setTabIcon(index, QIcon(resourceName));
    }
}



/**
 * @brief JenkinsJobMonitorWidget::jobStateChanged - Called by A JenkinsRequest when the job this Widget represents changes states.
 * @param jobName The name of the Job
 * @param buildNumber The build number of the job
 * @param activeConfiguration The active configuration
 * @param jobState The current State of the job.
 */
void JenkinsJobMonitorWidget::jobStateChanged(QString jobName, int buildNumber, QString activeConfiguration, JOB_STATE jobState)
{
    if(activeConfiguration == ""){
        this->jobName = jobName;
        this->buildNumber = buildNumber;
    }



    //Update the Job State GUI
    setJobState(activeConfiguration, jobState);

    //If we have got the configurations loaded we should request the console output of all of the ActiveConfigurations of this Job.
    if(configurations.size() > 0 && !requestedConsoleOutput){
        requestedConsoleOutput = true;
        //Setup visiblities
        loadingWidget->hideLoadingBar();
        titleWidget->setVisible(true);
        tabWidget->setVisible(true);

        //Get each ActiveConfigurations console output
        foreach(QString configuration, configurations){
            //Construct a JenkinsRequest Object to get the Console Output of this Configuration
            JenkinsRequest* jenkinsCO = jenkins->getJenkinsRequest(this);
            JenkinsRequest* jenkinsJS = jenkins->getJenkinsRequest(this);

             //Connect the emit signals from this to the JenkinsRequest Thread.
            connect(this, SIGNAL(getJobConsoleOutput(QString,int, QString)), jenkinsCO, SLOT(getJobConsoleOutput(QString,int,QString)));
            connect(this, SIGNAL(getJobState(QString,int,QString)), jenkinsJS, SLOT(getJobState(QString,int,QString)));

            //Connect the gotLiveCLIOutput SIGNAL to this so we can update the new GUI as the console output is written for this configuration.
            connect(jenkinsCO, SIGNAL(gotLiveCLIOutput(QString,int,QString,QString)), this, SLOT(gotJobConsoleOutput(QString,int,QString,QString)));

            //Connect the gotJobStateChange SIGNAL to this so we can update the GUI as the job state changes for this configuration.
            connect(jenkinsJS, SIGNAL(gotJobStateChange(QString,int,QString,JOB_STATE)), this, SLOT(jobStateChanged(QString,int,QString,JOB_STATE)));


            //Request the console output
            getJobConsoleOutput(this->jobName, this->buildNumber, configuration);

            if(configuration != ""){
                //Request the Job State
                getJobState(this->jobName, this->buildNumber, configuration);
            }

            //Disconnect the SIGNALs
            disconnect(this, SIGNAL(getJobConsoleOutput(QString,int, QString)), jenkinsCO, SLOT(getJobConsoleOutput(QString,int,QString)));
            disconnect(this, SIGNAL(getJobState(QString,int,QString)), jenkinsJS, SLOT(getJobState(QString,int,QString)));
        }
    }
}

/**
 * @brief JenkinsJobMonitorWidget::gotJobActiveConfigurations Called by A JenkinsRequest with the ActiveConfigurations for this job.
 * @param jobName The name of the Job
 * @param activeConfigurations The list of ActiveConfigurations.
 */
void JenkinsJobMonitorWidget::gotJobActiveConfigurations(QString jobName, QStringList activeConfigurations)
{
    configurations = activeConfigurations;

    //Construct a TextBrowser for each Active Configuration.
    foreach(QString configuration, configurations){
        QTextBrowser* newBrowser = new QTextBrowser();
        newBrowser->setEnabled(true);

        if(!configurationBrowsers.contains(configuration)){
            configurationBrowsers[configuration] = newBrowser;
        }

        //Get the Human Readable name of this Configuration
        QString tabName = configuration;
        if(configuration == ""){
            tabName = "Master";
        }else{
            int seperator = tabName.indexOf("=") + 1;
            tabName = tabName.mid(seperator);
        }

        //Add it to the Tabbed Widget.
        tabWidget->addTab(newBrowser, tabName);
    }
}

/**
 * @brief JenkinsJobMonitorWidget::gotJobConsoleOutput Called by A JenkinsRequest every time it's ActiveConfiguration gets new Console Output.
 * @param jobName The name of the Job
 * @param buildNumber The build number of the job.
 * @param activeConfiguration The Active Configuration the console Output relates to
 * @param consoleOutput The partial consoleOutput to append
 */
void JenkinsJobMonitorWidget::gotJobConsoleOutput(QString jobName, int buildNumber, QString activeConfiguration, QString consoleOutput)
{
    //Check if the jobName and buildNumber match.
    if(this->jobName == jobName && buildNumber == buildNumber){
        //Append the consoleOutput to the end of the QTextBrowser which matches the activeConfiguration.
        if(configurationBrowsers.contains(activeConfiguration)){
            QTextBrowser* output = configurationBrowsers[activeConfiguration];
            output->moveCursor (QTextCursor::End);
            output->insertPlainText (consoleOutput);
            output->moveCursor (QTextCursor::End);
        }
    }
}
