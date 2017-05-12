#include "jenkinsjobmonitorwidget.h"

#include "../../Controllers/JenkinsManager/jenkinsmanager.h"
#include "../../Controllers/JenkinsManager/jenkinsrequest.h"
#include "../../Controllers/ActionController/actioncontroller.h"
//#include "../../Utils/rootaction.h"
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
#include <QMovie>
#include <QStringBuilder>
#include <QAction>
#include <QWidget>


/**
 * @brief JenkinsJobMonitorWidget::JenkinsJobMonitorWidget A Widget which represents and displays the Output of a Jenkins Job which has just been invoked.
 * @param parent The Parent Widget for this Dialog
 * @param jenkins The Jenkins Manager used to spawn requests.
 * @param jobName The Name of the job started.
 */

JenkinsJobMonitorWidget::JenkinsJobMonitorWidget(QWidget *parent, JenkinsManager *jenkins):QDialog(parent)
{
    this->jenkins = jenkins;
    this->job_build = -1;
    this->movie_spinning = 0;

    setupLayout();
    themeChanged();

    connect(Theme::theme(), &Theme::theme_Changed, this, &JenkinsJobMonitorWidget::themeChanged);
}

/**
 * @brief JenkinsJobMonitorWidget::~JenkinsJobMonitorWidget Destructor
 */
JenkinsJobMonitorWidget::~JenkinsJobMonitorWidget()
{
    //Do Nothing.
}

/**
 * @brief JenkinsJobMonitorWidget::setupLayout Sets up the Layout for the Widget.
 */
void JenkinsJobMonitorWidget::setupLayout()
{
    //Construct a Vertical Layout.
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);

    //Setup Title.
    QHBoxLayout* title_layout = new QHBoxLayout();
    verticalLayout->addLayout(title_layout);

    //Set up a QLabel for the name of the Jenkins Job
    job_label = new QLabel("Jenkins Job", this);
    job_icon = new QLabel(this);
    job_icon->setFixedSize(32,32);

    title_layout->addWidget(job_icon);
    title_layout->addWidget(job_label);
    title_layout->addStretch();

    //Setup the Title
    tabWidget = new QTabWidget();
    verticalLayout->addWidget(tabWidget,1);

    action_toolbar = new QToolBar(this);
    action_toolbar->setIconSize(QSize(20,20));
    action_toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    build_action = jenkins->GetActionController()->jenkins_executeJob->constructSubAction();
    action_toolbar->addAction(build_action);

    verticalLayout->addWidget(action_toolbar, 0, Qt::AlignRight);

    tabWidget->setTabsClosable(true);
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
}

/**
 * @brief JenkinsJobMonitorWidget::setJobState Changes the Icon of the jobIcon to represent the current Job State.
 * @param activeConfiguration The name of the ActiveConfiguration the job state is for
 * @param state
 */
void JenkinsJobMonitorWidget::setJobState(QString jobName, int buildNumber, QString config, JOB_STATE state)
{
    QString prefixName = "Icons";
    QString resourceName = "";

    //Get the name of the tab
    auto tab_name = getTabName(config, buildNumber);
    auto monitor = consoleMonitors.value(tab_name, 0);
    bool is_master = config == "";
    if(monitor){
        monitor->state = state;

        switch(state){
            case BUILDING:{
                if(is_master && !movie_spinning){
                    movie_spinning = new QMovie(this);
                    movie_spinning->setFileName(":/Images/Icons/loading");
                    movie_spinning->setScaledSize(QSize(32,32));
                    movie_spinning->start();
                    connect(movie_spinning, &QMovie::frameChanged, this, &JenkinsJobMonitorWidget::frameChanged);
                }
                job_icon->setMovie(movie_spinning);
                job_label->setText(job_name + " #" + QString::number(job_build));

                break;
            }
            case BUILT:{
                resourceName = "sphereBlue";
                break;
            }
            case FAILED:{
                resourceName = "sphereRed";
                break;
            }
            case ABORTED:{
                resourceName = "sphereGray";
                break;
            }
            default:
                break;
        }

        //Update and hide stop button
        monitor->stop_job->setEnabled(state == BUILDING);

        int tab_index = getTabIndex(tab_name);
        if(tab_index >= 0 && resourceName != ""){
            tabWidget->setTabIcon(tab_index, Theme::theme()->getImage(prefixName, resourceName));
            if(config == "" && job_icon){
                job_icon->setPixmap(Theme::theme()->getImage(prefixName, resourceName, QSize(32,32)));
            }
        }
    }
}



/**
 * @brief JenkinsJobMonitorWidget::jobStateChanged - Called by A JenkinsRequest when the job this Widget represents changes states.
 * @param jobName The name of the Job
 * @param buildNumber The build number of the job
 * @param activeConfiguration The active configuration
 * @param jobState The current State of the job.
 */
void JenkinsJobMonitorWidget::gotJobStateChange(QString job_name, int job_build, QString activeConfiguration, JOB_STATE jobState)
{
    //We should request
    if(activeConfiguration == "" && jobState == BUILDING){
        auto configs = jenkins->GetJobConfigurations(job_name);
        setupTabs(job_name, job_build, configs);
    }

    //Update the Job State GUI
    setJobState(job_name, job_build, activeConfiguration, jobState);
}


QString JenkinsJobMonitorWidget::getTabName(QString configuration, int build_number){
    //Get the Human Readable name of this Configuration
    QString tabName = configuration;
    if(configuration != ""){
        int seperator = tabName.indexOf("=") + 1;
        tabName = tabName.mid(seperator);
    }
    if(build_number > 0){
        if(configuration != ""){
            tabName += "[";
        }
        tabName += "#" + QString::number(build_number);
        if(configuration != ""){
            tabName += "]";
        }
    }
    return tabName;
}

/**
 * @brief JenkinsJobMonitorWidget::gotJobActiveConfigurations Called by A JenkinsRequest with the ActiveConfigurations for this job.
 * @param jobName The name of the Job
 * @param activeConfigurations The list of ActiveConfigurations.
 */
void JenkinsJobMonitorWidget::setupTabs(QString job_name, int job_build, QStringList configurations)
{
    //Set the current job name/build
    this->job_name = job_name;
    this->job_build = job_build;

    //Construct a TextBrowser for each Active Configuration.
    foreach(QString config, configurations){
        //Get the Human Readable name of this Configuration
        QString tab_name = getTabName(config, job_build);

        if(!consoleMonitors.contains(tab_name)){
            auto monitor = new ConsoleMonitor();

            monitor->browser = new QTextBrowser(this);
            auto browser = monitor->browser;
            monitor->job_name = job_name;
            monitor->job_number = job_build;
            monitor->configuration_name = config;

            monitor->state = NO_JOB;

            browser->setEnabled(true);
            browser->setOpenExternalLinks(true);
            consoleMonitors[tab_name] = monitor;

            auto widget = new QWidget(this);
            QVBoxLayout* verticalLayout = new QVBoxLayout(widget);
            verticalLayout->addWidget(browser, 1);
            QToolBar* toolbar = new QToolBar(widget);
            toolbar->setIconSize(QSize(20,20));
            toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

            verticalLayout->addWidget(toolbar);

            auto stop_action = toolbar->addAction("Stop Job");
            auto jenkins_action = toolbar->addAction("Open Jenkins");
            stop_action->setIcon(Theme::theme()->getIcon("Icons", "circleCrossDark"));
            jenkins_action->setIcon(Theme::theme()->getIcon("Icons", "globe"));
            monitor->stop_job = stop_action;
            monitor->jenkins_action = jenkins_action;







            connect(stop_action, &QAction::triggered, this, [this, monitor](){_stopJob(monitor->job_name, monitor->job_number, monitor->configuration_name);});
            connect(jenkins_action, &QAction::triggered, this, [this, monitor](){_gotoJenkinsURL(monitor->job_name, monitor->job_number, monitor->configuration_name);});

            tabWidget->addTab(widget, tab_name);

            //Construct a JenkinsRequest Object to get the Console Output of this Configuration
            JenkinsRequest* console_request = jenkins->GetJenkinsRequest(browser);

            //Connect the emit signals from this to the JenkinsRequest Thread.
            connect(this, &JenkinsJobMonitorWidget::getJobConsoleOutput, console_request, &JenkinsRequest::GetJobConsoleOutput);

            //Connect the return signals
            connect(console_request, &JenkinsRequest::GotJobStateChange, this, &JenkinsJobMonitorWidget::gotJobStateChange);
            connect(console_request, &JenkinsRequest::GotLiveJobConsoleOutput, this, &JenkinsJobMonitorWidget::gotJobConsoleOutput);

            //Get the job info
            if(config != ""){
                emit getJobConsoleOutput(job_name, job_build, config);
            }

            //Disconnect the request signals
            disconnect(this, &JenkinsJobMonitorWidget::getJobConsoleOutput, console_request, &JenkinsRequest::GetJobConsoleOutput);

        }
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
    //Get the name of the tab
    QString tab_name = getTabName(activeConfiguration, buildNumber);
    auto monitor = consoleMonitors.value(tab_name, 0);
    if(monitor){
        auto browser = monitor->browser;
        //Append the HTML
        browser->moveCursor (QTextCursor::End);
        browser->insertHtml(htmlize(consoleOutput));
        browser->moveCursor (QTextCursor::End);
    }
}

void JenkinsJobMonitorWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getWidgetStyleSheet("JenkinsJobMonitorWidget") % theme->getGroupBoxStyleSheet() % theme->getScrollBarStyleSheet() % theme->getLabelStyleSheet() % theme->getWidgetStyleSheet("QTextBrowser"));
    action_toolbar->setStyleSheet(theme->getToolBarStyleSheet());
    job_label->setStyleSheet(theme->getTitleLabelStyleSheet());

    build_action->setIcon(theme->getIcon("Icons", "jobBuild"));


    tabWidget->setStyleSheet(theme->getTabbedWidgetStyleSheet() +
                             "QTabBar::tab:selected{ background:" % theme->getPressedColorHex() % "; color:" % theme->getTextColorHex(Theme::CR_SELECTED) % ";}"
                             "QTabBar::tab:hover{ background:" % theme->getHighlightColorHex() % ";}");

    foreach(auto monitor, consoleMonitors.values()){
        monitor->stop_job->setIcon(theme->getIcon("Icons", "circleCrossDark"));
        monitor->jenkins_action->setIcon(theme->getIcon("Icons", "globe"));
    }
}

void JenkinsJobMonitorWidget::_stopJob(QString jobName, int buildNumber, QString activeConfiguration)
{
    auto tab_name = getTabName(activeConfiguration, buildNumber);
    auto monitor = consoleMonitors.value(tab_name, 0);
    if(monitor && monitor->state == BUILDING){
        //Construct a new JenkinsRequest Object.
        JenkinsRequest* jenkinsStop = jenkins->GetJenkinsRequest(this);
        auto r = connect(this, &JenkinsJobMonitorWidget::stopJob, jenkinsStop, &JenkinsRequest::StopJob);
        emit stopJob(jobName, buildNumber, activeConfiguration);
        disconnect(r);
    }
}

void JenkinsJobMonitorWidget::_gotoJenkinsURL(QString jobName, int buildNumber, QString activeConfiguration)
{
    if(activeConfiguration == ""){
        auto url = jenkins->GetUrl() + "job/" + jobName + "/" + QString::number(buildNumber);
        emit gotoURL(url);
    }
}

void JenkinsJobMonitorWidget::frameChanged(int frame)
{
    Q_UNUSED(frame);
    if(movie_spinning){
        QPixmap pixmap = movie_spinning->currentPixmap();

        //Update the icons for all console tabs which are currently building
        foreach(QString key, consoleMonitors.keys()){
            auto monitor = consoleMonitors[key];
            if(monitor && monitor->state == BUILDING){
                int i = getTabIndex(key);
                tabWidget->setTabIcon(i, QIcon(pixmap));
            }
        }
    }
}

void JenkinsJobMonitorWidget::closeTab(int tabID)
{
    QString tabName = tabWidget->tabText(tabID);
    auto config_browser = consoleMonitors.value(tabName, 0);
    if(config_browser){
        consoleMonitors.remove(tabName);
        tabWidget->removeTab(tabID);
        //Clean memory
        delete config_browser->browser;
        delete config_browser;
    }
}

int JenkinsJobMonitorWidget::getTabIndex(QString tab_name)
{
    int tab_index = -1;
    for(int i=0; i < tabWidget->count(); i++){
        if(tabWidget->tabText(i) == tab_name){
            tab_index = i;
            break;
        }
    }
    return tab_index;
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
