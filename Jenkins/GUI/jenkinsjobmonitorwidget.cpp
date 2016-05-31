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
#include "../../View/theme.h"
#define CONSOLE_SUFFIX "_CONSOLE"
#define STATE_SUFFIX "_STATE"

/**
 * @brief JenkinsJobMonitorWidget::JenkinsJobMonitorWidget A Widget which represents and displays the Output of a Jenkins Job which has just been invoked.
 * @param parent The Parent Widget for this Dialog
 * @param jenkins The Jenkins Manager used to spawn requests.
 * @param jobName The Name of the job started.
 */

JenkinsJobMonitorWidget::JenkinsJobMonitorWidget(QWidget *parent, JenkinsManager *jenkins, QString jobName, Jenkins_JobParameters build_parameters):QDialog(parent)
{
    setMinimumWidth(800);
    setMinimumHeight(600);


    this->buildParameters = build_parameters;
    this->jenkins = jenkins;
    this->jobName = jobName;
    this->buildNumber = -1;
    loadingWidget = 0;
    areTabsSpinning = false;
    spinning = new QMovie(this);
    spinning->setFileName(":/Actions/Waiting.gif");
    spinning->start();

    requestedConsoleOutput = false;

    setWindowTitle("Jenkins Job Monitor");
    setWindowIcon(Theme::theme()->getIcon("Actions", "Job_Build"));


    setStyleSheet("QDialog{background-color:white; color: #333;}");

    setupLayout();

    if(jenkins->hasValidatedSettings()){
        loadingWidget->setWaiting(true);
    }else{
        connect(jenkins, SIGNAL(settingsValidationComplete(bool, QString)), this, SLOT(authenticationFinished(bool, QString)));
    }

    //Request the JenkinsData
    getJenkinsData();


    for(int i =0; i < 255;i++){
        buildingTabs[i] = false;
    }

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
    titleLayout = new QHBoxLayout();
    titleWidget->setLayout(titleLayout);

    //Set up a QLabel for the Building Icon

    jobIcon = new QLabel();
    jobIcon->setPixmap(Theme::theme()->getImage("Actions", "Job_Build"));
    jobIcon->setFixedSize(48,48);

    //Setup a QPushButton to stop the job.
    cancelButton = new QPushButton("");
    cancelButton->setIcon(Theme::theme()->getImage("Actions", "Job_Stop"));
    cancelButton->setStyleSheet("border: 0px solid black;");
    cancelButton->setFixedSize(QSize(24,24));
    cancelButton->setToolTip("Cancel the Job.");
    connect(cancelButton, SIGNAL(pressed()), this, SLOT(cancelPressed()));

    stopButton = new QPushButton("Stop Job");
    stopButton->setObjectName(THEME_STYLE_QPUSHBUTTON_JENKINS);
    stopButton->setToolTip("Stop the Job.");

    //Given a length of 0
    if(getBuildParameter("RUNTIME_DURATION") == "0"){
        //Show stop Job button only for indefinite jobs
        stopButton->setVisible(true);
    }else{
        //Hide stop Job button
        stopButton->setVisible(false);
    }

    cancelLabel = new QLabel();
    cancelLabel->setFixedSize(QSize(16,16));
    cancelLabel->setMovie(spinning);
    cancelLabel->setVisible(false);

    connect(stopButton, SIGNAL(pressed()), this, SLOT(stopPressed()));


    //Setup a QLabel for the Job Name
    //Get URL.

    jobLabel = new QLabel();
    QString url = jenkins->getJenkinsURL(jobName);
    jobLabel->setText("<a href='" + url + "'>" + jobName + "</a>");

    jobLabel->setStyleSheet("font-family: Helvetica, Arial, sans-serif; font-size: 18px;  font-weight: bold;");
    jobLabel->setTextFormat(Qt::RichText);
    jobLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    jobLabel->setOpenExternalLinks(true);


    titleLayout->addWidget(jobIcon);
    titleLayout->addWidget(jobLabel,1);
    titleLayout->addWidget(stopButton);
    //titleLayout->addWidget(cancelLabel);
    titleLayout->addWidget(cancelButton);
    //titleLayout->addStretch();

    verticalLayout->addWidget(titleWidget);

    //Setup the Title
    tabWidget = new QTabWidget();
    verticalLayout->addWidget(tabWidget,1);
    tabWidget->setTabsClosable(true);
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

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
    QString prefixName = "Actions";

    int index = configurations.indexOf(activeConfiguration);

    if(state == BUILDING){
        if(activeConfiguration == ""){
            QMovie* movie = new QMovie(this);
            movie->setFileName(":/Actions/Job_Building.gif");
            movie->start();

            jobIcon->setMovie(movie);
            QString url = jenkins->getJenkinsURL(jobName, buildNumber);
            jobLabel->setText("<a href='" + url + "'>" + jobName + " Build #" + QString::number(buildNumber) + "</a>");

            if(!areTabsSpinning){
                connect(spinning, SIGNAL(frameChanged(int)), this, SLOT(frameChanged(int)));
                areTabsSpinning = true;
            }
        }
        buildingTabs[index] = true;
    }else if(state == BUILT){
        resourceName = "Job_Built";
        buildingTabs[index] = false;
    }else if(state == FAILED){
        resourceName = "Job_Failed";
        buildingTabs[index] = false;
    }else if(state == ABORTED){
        resourceName = "Job_Aborted";
        buildingTabs[index] = false;
    }

    if(index >=0 && resourceName != ""){
        if(activeConfiguration == ""){
            jobIcon->setPixmap(Theme::theme()->getImage(prefixName, resourceName));
            //Hide the Stop Button.
            cancelButton->setVisible(false);
            stopButton->setVisible(false);
            cancelLabel->setVisible(false);
        }
        tabWidget->setTabIcon(index, Theme::theme()->getImage(prefixName, resourceName));
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
            QObject *threadParent = this;
            QTextBrowser* textBrowser = configurationBrowsers[configuration];
            if(textBrowser){
                threadParent = textBrowser;
            }

            //Construct a JenkinsRequest Object to get the Console Output of this Configuration
            JenkinsRequest* jenkinsCO = jenkins->getJenkinsRequest(threadParent);
            JenkinsRequest* jenkinsJS = jenkins->getJenkinsRequest(threadParent);

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
    Q_UNUSED(jobName);
    configurations = activeConfigurations;

    //Construct a TextBrowser for each Active Configuration.
    foreach(QString configuration, configurations){
        QTextBrowser* newBrowser = new QTextBrowser();


        QFont font("Monospace");
        font.setStyleHint(QFont::Monospace);
        font.setPixelSize(12);
        newBrowser->setFont(font);

        //newBrowser->setStyleSheet("QTextBrowser{font-family:Inconsolata;font-size:14px;line-height:1.4em;}");
        newBrowser->setEnabled(true);
        newBrowser->setOpenExternalLinks(true);

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
    if(this->jobName == jobName && this->buildNumber == buildNumber){
        //Append the consoleOutput to the end of the QTextBrowser which matches the activeConfiguration.
        if(configurationBrowsers.contains(activeConfiguration)){
            QTextBrowser* output = configurationBrowsers[activeConfiguration];

            output->moveCursor (QTextCursor::End);
            output->insertHtml(htmlize(consoleOutput));
            output->moveCursor (QTextCursor::End);
        }
    }
}

void JenkinsJobMonitorWidget::cancelPressed()
{
    if(this->jobName != ""  && buildNumber > 0){
        haltRequested(cancelButton);
        //Construct a new JenkinsRequest Object.
        JenkinsRequest* jenkinsStop = jenkins->getJenkinsRequest(this);
        //Connect the emit signals from this Thread to the JenkinsRequest Thread.
        connect(this, SIGNAL(cancelJob(QString,int,QString)), jenkinsStop, SLOT(stopJob(QString,int,QString)));
        emit cancelJob(jobName, buildNumber, "");
        disconnect(this, SIGNAL(cancelJob(QString,int,QString)), jenkinsStop, SLOT(stopJob(QString,int,QString)));
    }
}

void JenkinsJobMonitorWidget::stopPressed()
{
    if(this->jobName != ""  && buildNumber > 0){
        //Get Index in layout.
        haltRequested(stopButton);

        QString stopJobName = jobName + "-Stop";

        Jenkins_JobParameters buildParameters;

        //Get the values from each of the Parameter Widgets.
        Jenkins_Job_Parameter buildParameter;
        buildParameter.name = "build";
        buildParameter.value = jobName + "#" + QString::number(buildNumber);
        buildParameters.append(buildParameter);

        //Get a new Jenkins Request Object. Tied to the new Jenkins Widget
        JenkinsRequest* jenkinsStop = jenkins->getJenkinsRequest(this);

        connect(this, SIGNAL(stopJob(QString,Jenkins_JobParameters)), jenkinsStop, SLOT(buildJob(QString,Jenkins_JobParameters)));
        emit stopJob(stopJobName, buildParameters);
        disconnect(this, SIGNAL(stopJob(QString,Jenkins_JobParameters)), jenkinsStop, SLOT(buildJob(QString,Jenkins_JobParameters)));
    }
}

void JenkinsJobMonitorWidget::frameChanged(int frame)
{
    Q_UNUSED(frame);
    if(areTabsSpinning && spinning){
        QPixmap pixmap = spinning->currentPixmap();
        for(int i = 0; i < tabWidget->count(); i++){
            if(buildingTabs[i]){
                tabWidget->setTabIcon(i, QIcon(pixmap));
            }
        }
    }
}

void JenkinsJobMonitorWidget::closeTab(int tabID)
{

    QString tabName = tabWidget->tabText(tabID);
    if(tabName != "Master"){
        QString configName = "master=" + tabName;
        QTextBrowser* textBrowser = configurationBrowsers[configName];
        configurationBrowsers.remove(configName);
        tabWidget->removeTab(tabID);
        if(textBrowser){
            delete textBrowser;
        }
    }
}

void JenkinsJobMonitorWidget::authenticationFinished(bool success, QString message)
{
    Q_UNUSED(message)
    loadingWidget->authenticationFinished();
    if(!success){
        reject();
    }
}

QString JenkinsJobMonitorWidget::getBuildParameter(QString name)
{
    foreach(Jenkins_Job_Parameter parameter, buildParameters){
        if(parameter.name == name){
            return parameter.value;
        }
    }
    return QString();
}

void JenkinsJobMonitorWidget::haltRequested(QPushButton* button)
{
    if(button){
        titleLayout->removeWidget(cancelLabel);
        int position = titleLayout->indexOf(button);
        titleLayout->insertWidget(position, cancelLabel);
    }
    cancelLabel->setVisible(true);
    cancelButton->setEnabled(false);
    stopButton->setEnabled(false);
}

QString JenkinsJobMonitorWidget::htmlize(QString consoleOutput)
{
    QRegExp regExp("(https?://\\S+)");
    consoleOutput = consoleOutput.replace(regExp, "<a href='\\1'>\\1</a>" );
    consoleOutput = consoleOutput.replace("\r\n", "<br />");
    consoleOutput = consoleOutput.replace("\n", "<br />");
    //Match URLs
    return consoleOutput;
}
