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
#include <QStringBuilder>
#include "../../../theme.h"

#define CONSOLE_SUFFIX "_CONSOLE"
#define STATE_SUFFIX "_STATE"

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
    action_toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    build_action = action_toolbar->addAction("Build Job");
    stop_action = action_toolbar->addAction("Stop Job");

    //connect(build_action, &QAction::triggered, jenkins, &JenkinsManager::executeJenkinsJob);
    connect(stop_action, &QAction::triggered, this, &JenkinsJobMonitorWidget::stopPressed);

    verticalLayout->addWidget(action_toolbar, 0, Qt::AlignRight);

    tabWidget->setTabsClosable(true);
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

    build_action->setVisible(true);
    stop_action->setVisible(false);
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
                    qCritical() << "Loading movie!";
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
        }

        //Update and hide stop button
        if(is_master){
            stop_action->setVisible(state == BUILDING);
            build_action->setVisible(state != BUILDING);
        }

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
        auto configs = jenkins->getActiveConfigurations(job_name);
        setupTabs(job_name, job_build, configs);
    }

    qCritical() << job_name << " "<< job_build << " S" << jobState << " " << activeConfiguration << " ";

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
            qCritical() <<   "Constructing Tab!";
            auto monitor = new ConsoleMonitor();
            monitor->browser = new QTextBrowser(this);
            auto browser = monitor->browser;

            monitor->state = NO_JOB;

            browser->setEnabled(true);
            browser->setOpenExternalLinks(true);
            consoleMonitors[tab_name] = monitor;
            tabWidget->addTab(browser, tab_name);

            //Construct a JenkinsRequest Object to get the Console Output of this Configuration
            JenkinsRequest* console_request = jenkins->getJenkinsRequest(browser);
            JenkinsRequest* state_request = jenkins->getJenkinsRequest(browser);

            //Connect the emit signals from this to the JenkinsRequest Thread.
            connect(this, &JenkinsJobMonitorWidget::getJobConsoleOutput, console_request, &JenkinsRequest::getJobConsoleOutput);
            connect(this, &JenkinsJobMonitorWidget::getJobState, state_request, &JenkinsRequest::getJobState);

            //Connect the return signals
            connect(state_request, &JenkinsRequest::gotJobStateChange, this, &JenkinsJobMonitorWidget::gotJobStateChange);
            connect(console_request, &JenkinsRequest::gotLiveCLIOutput, this, &JenkinsJobMonitorWidget::gotJobConsoleOutput);

            //Get the job info
            qCritical() << "getJobConsoleOutput(" << job_name << ", " << job_build << ", " << config << ")";
            emit getJobConsoleOutput(job_name, job_build, config);

            if(config != ""){
                qCritical() << "getJobState(" << job_name << ", " << job_build << ", " << config << ")";
                //Request the Job State
                emit getJobState(job_name, job_build, config);
            }

            //Disconnect the request signals
            disconnect(this, &JenkinsJobMonitorWidget::getJobConsoleOutput, console_request, &JenkinsRequest::getJobConsoleOutput);
            disconnect(this, &JenkinsJobMonitorWidget::getJobState, state_request, &JenkinsRequest::getJobState);
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
    stop_action->setIcon(theme->getIcon("Icons", "jenkinsStop"));

    tabWidget->setStyleSheet(theme->getTabbedWidgetStyleSheet() +
                             "QTabBar::tab:selected{ background:" % theme->getPressedColorHex() % "; color:" % theme->getTextColorHex(Theme::CR_SELECTED) % ";}"
                             "QTabBar::tab:hover{ background:" % theme->getHighlightColorHex() % ";}");
}

void JenkinsJobMonitorWidget::stopPressed()
{
    if(job_name != "" && job_build > 0){
        //Construct a new JenkinsRequest Object.
        JenkinsRequest* jenkinsStop = jenkins->getJenkinsRequest(this);

        connect(this, &JenkinsJobMonitorWidget::stopJob, jenkinsStop, &JenkinsRequest::stopJob);

        emit stopJob(job_name, job_build, "");

        disconnect(this, &JenkinsJobMonitorWidget::stopJob, jenkinsStop, &JenkinsRequest::stopJob);
    }
}

void JenkinsJobMonitorWidget::frameChanged(int frame)
{
    Q_UNUSED(frame);
    if(movie_spinning){
        QPixmap pixmap = movie_spinning->currentPixmap();

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
