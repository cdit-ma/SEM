#include "jobmonitor.h"
#include "consolemonitor.h"
#include "jenkinsmonitor.h"
#include "../../theme.h"
#include <QBoxLayout>

JobMonitor::JobMonitor(JenkinsManager* jenkins_manager,QWidget *parent){
    this->jenkins_manager = jenkins_manager;
    setupLayout();

    auto theme = Theme::theme();
    //Set some alias
    theme->setIconAlias("Jenkins", Notification::getSeverityString(Notification::Severity::ERROR), "Icons", "sphereRed");
    theme->setIconAlias("Jenkins", Notification::getSeverityString(Notification::Severity::INFO), "Icons", "sphereGray");
    theme->setIconAlias("Jenkins", Notification::getSeverityString(Notification::Severity::SUCCESS), "Icons", "sphereBlue");
    theme->setIconAlias("Jenkins", Notification::getSeverityString(Notification::Severity::RUNNING), "Icons", "running");
    theme->setIconAlias("Jenkins", Notification::getSeverityString(Notification::Severity::NONE), "Icons", "sphereGray");



    connect(theme, &Theme::theme_Changed, this, &JobMonitor::themeChanged);
    themeChanged();

    connect(jenkins_manager, &JenkinsManager::gotRecentJobs, this, &JobMonitor::gotRecentJobs);

    connect(jenkins_manager, &JenkinsManager::gotJobArtifacts, this, &JobMonitor::gotJenkinsJobArtifacts);
    connect(jenkins_manager, &JenkinsManager::gotJobStateChange, this, &JobMonitor::gotJenkinsJobStateChange);
    connect(jenkins_manager, &JenkinsManager::gotJobConsoleOutput, this, &JobMonitor::gotJenkinsJobConsoleOutput);
    connect(jenkins_manager, &JenkinsManager::BuildingJob, this, [=](QString job_name){
        jenkins_manager->GetRecentJobs(job_name);
    });

    refreshRecentJobs();
}

QToolBar* JobMonitor::getToolbar(){
    return toolbar;
}

JenkinsMonitor* JobMonitor::constructJenkinsMonitor(QString job_name, int build_number){
    auto lookup_key = getJobKey(job_name, build_number);
    if(!monitors.contains(lookup_key)){
        auto monitor = new JenkinsMonitor(job_name, build_number, this);
        stacked_widget->addWidget(monitor);
        monitors[lookup_key] = monitor;
        connect(monitor, &Monitor::StateChanged, this, &JobMonitor::MonitorStateChanged);
        connect(monitor, &Monitor::Close, this, &JobMonitor::MonitorClose);
        
        return monitor;
    }
    return 0;
}

ConsoleMonitor* JobMonitor::constructConsoleMonitor(){
    QString name = "Local Deployment";
    auto lookup_key = getJobKey(name, 0);

    if(!monitors.contains(lookup_key)){
        if(!running_jobs_box->gotOption(lookup_key)){
            running_jobs_box->addOption(lookup_key, name, "Icons", "circleQuestion", false);
        }

        auto monitor = new ConsoleMonitor(this);
        stacked_widget->addWidget(monitor);
        monitors[lookup_key] = monitor;
        connect(monitor, &Monitor::StateChanged, this, &JobMonitor::MonitorStateChanged);
        return monitor;
    }
    return 0;
}

Monitor* JobMonitor::getMonitor(QString job_name, int build_number){
    auto lookup_key = getJobKey(job_name, build_number);
    return monitors.value(lookup_key, 0);
};

ConsoleMonitor* JobMonitor::getConsoleMonitor(QString name){
    return qobject_cast<ConsoleMonitor*>(getMonitor(name, 0));
};

JenkinsMonitor* JobMonitor::getJenkinsMonitor(QString name, int build_number){
    return qobject_cast<JenkinsMonitor*>(getMonitor(name, build_number));
};




void JobMonitor::updateMonitorIcon(Monitor* monitor, Theme* theme){
    
    if(monitor){
        if(!theme){
            theme = Theme::theme();
        }

        auto jenkins_monitor = qobject_cast<JenkinsMonitor*>(monitor);
        auto state = monitor->getState();

        int job_id = -1;
        QString job_name = "local";

        if(jenkins_monitor){
            job_id = jenkins_monitor->getBuildNumber();
            job_name = jenkins_monitor->GetJobName();
        }else{
            job_id = 0;
            job_name = "Local Deployment";
        }
        updateMonitorIcon(job_name, job_id, state);
    }
}

void JobMonitor::updateMonitorIcon(QString job_name, int job_number, Notification::Severity state){
    auto key = getJobKey(job_name, job_number);
    running_jobs_box->updateOptionIcon(key, "Jenkins", Notification::getSeverityString(state));
}

void JobMonitor::MonitorStateChanged(Notification::Severity state){
    auto monitor = qobject_cast<Monitor*>(sender());
    updateMonitorIcon(monitor);
};

void JobMonitor::MonitorClose(){
    auto monitor = qobject_cast<Monitor*>(sender());
    if(monitor){
        auto key = monitors.key(monitor);
        monitors.remove(key);
        stacked_widget->removeWidget(monitor);
        monitor->deleteLater();
    }
};

void JobMonitor::stackedWidgetChanged(int index){
    auto widget = stacked_widget->widget(index);
    auto monitor = qobject_cast<Monitor*>(widget);
    if(monitor){
        auto key = monitors.key(monitor);
        running_jobs_box->setOptionChecked(key, true);
    }else{
        running_jobs_box->reset(false);
    }
}


void JobMonitor::setupLayout(){

    auto left_widget = new QWidget(this);
    auto right_widget = new QWidget(this);

    {
        //LEFT WIDGET
        left_widget->setContentsMargins(5,5,1,5);
        auto v_layout = new QVBoxLayout(left_widget);
        v_layout->setMargin(0);

        jobs_list_widget = new QWidget(this);
        jobs_list_widget->setContentsMargins(5,0,5,5);
        jobs_list_layout = new QVBoxLayout(jobs_list_widget);
        jobs_list_layout->setAlignment(Qt::AlignTop);
        jobs_list_layout->setMargin(0);
        jobs_list_layout->setSpacing(0);

        jobs_list_scroll = new QScrollArea(this);
        jobs_list_scroll->setWidget(jobs_list_widget);
        jobs_list_scroll->setWidgetResizable(true);

        jobs_list_widget->setObjectName("BLANK_WIDGET");


        v_layout->addWidget(jobs_list_scroll, 1);
    }

    {
        //RIGHT WIDGET
        right_widget->setContentsMargins(1,5,5,5);
        auto v_layout = new QVBoxLayout(right_widget);
        v_layout->setMargin(0);
        v_layout->setSpacing(5);

        stacked_widget = new QStackedWidget(this);
        stacked_widget->setContentsMargins(QMargins(5,5,5,5));

        toolbar = new QToolBar(this);
        toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    
        v_layout->addWidget(stacked_widget, 1);
        v_layout->addWidget(toolbar, 0, Qt::AlignRight);

        connect(stacked_widget, &QStackedWidget::currentChanged, this, &JobMonitor::stackedWidgetChanged);
    }

    splitter = new QSplitter(this);
    splitter->addWidget(left_widget);
    splitter->addWidget(right_widget);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes(QList<int>() << 250 << 350);

    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(splitter, 1);

    {
        running_jobs_box = new OptionGroupBox("Jobs", this);
        running_jobs_box->setExclusive(true);
        running_jobs_box->setCheckable(false);
        

        running_jobs_box->setResetButtonIcon("Icons", "refresh");
        running_jobs_box->setResetButtonText("Refresh Jenkins Jobs List");
        jobs_list_layout->addWidget(running_jobs_box);

        connect(running_jobs_box, &OptionGroupBox::checkedOptionsChanged, this, &JobMonitor::jobPressed);
        connect(running_jobs_box, &OptionGroupBox::resetPressed, this, &JobMonitor::refreshRecentJobs);
    }
};

void JobMonitor::gotRecentJobs(QList<Jenkins_Job_Status> recent_jobs){

    std::reverse(recent_jobs.begin(), recent_jobs.end());
    for(auto job : recent_jobs){
        auto key = getJobKey(job.name, job.number);

        auto title = job.name + " #" + QString::number(job.number);

        if(!running_jobs_box->gotOption(key)){
            running_jobs_box->addOption(key, title, "Icons", "circleQuestion", true);
        }else{
            running_jobs_box->updateOptionLabel(key, title);
        }

        updateMonitorIcon(job.name, job.number, job.state);
    }
}

QPair<QString, int> JobMonitor::splitJobKey(QString key){
    int last_index = key.lastIndexOf("#");
    auto left = key.left(last_index);
    auto right = key.mid(last_index + 1);
    bool ok;
    int id = right.toInt(&ok);
    if(!ok){
        id = -1;
    }

    return qMakePair(left, id);
}   

QString JobMonitor::getJobKey(QString job_name, int job_number){
    return job_name + "#" + QString::number(job_number);
}

void JobMonitor::refreshRecentJobs(){
    if(jenkins_manager){
        jenkins_manager->GetRecentJobs(jenkins_manager->GetJobName());
    }
}

void JobMonitor::jobPressed(){
    
    QString job_key;

    if(running_jobs_box){
        auto jobs_selected = running_jobs_box->getCheckedOptions<QString>().toSet();

        if(!running_jobs_box->isResetChecked()){
            job_key = *jobs_selected.constBegin();
        }
    }

    auto split_key = splitJobKey(job_key);

    if(split_key.second >= 0){
        requestJobConsoleOutput(split_key.first, split_key.second);
    }
}

void JobMonitor::requestJobConsoleOutput(QString job_name, int job_id){
    auto job_monitor = getMonitor(job_name, job_id);

    if(!job_monitor){
        //Has to be Jenkins
        auto jenkins_job_monitor = constructJenkinsMonitor(job_name, job_id);

        if(jenkins_manager){
            connect(jenkins_job_monitor, &JenkinsMonitor::Abort, [=](){jenkins_manager->AbortJob(job_name, job_id);});
            connect(jenkins_job_monitor, &JenkinsMonitor::GotoURL, [=](){jenkins_manager->GotoJob(job_name, job_id);});
            jenkins_manager->GetJobConsoleOutput(job_name, job_id);
        }
        job_monitor = jenkins_job_monitor;
    }

    if(job_monitor){
        stacked_widget->setCurrentWidget(job_monitor);
    }
}

void JobMonitor::themeChanged(){
    auto theme = Theme::theme();

    setStyleSheet(
                    "JobMonitor {background-color: " % theme->getBackgroundColorHex() + ";border:1px solid " % theme->getDisabledBackgroundColorHex() % ";}" +
                    "QScrollArea {border: 1px solid " % theme->getAltBackgroundColorHex() % "; background: rgba(0,0,0,0); } " +
                    "QLabel {color:" + theme->getTextColorHex() + ";} " + 
                    theme->getToolBarStyleSheet() +
                    theme->getSplitterStyleSheet() +
                    "QToolButton::checked:!hover{background: " % theme->getAltBackgroundColorHex() % ";}"
                );

    toolbar->setIconSize(theme->getIconSize());
    jobs_list_widget->setStyleSheet("QWidget#BLANK_WIDGET{background: rgba(0,0,0,0);}");

    
    for(auto monitor : monitors.values()){
        updateMonitorIcon(monitor);
    }
};


void JobMonitor::gotJenkinsJobStateChange(QString job_name, int job_build, QString, Notification::Severity job_state){
    auto jenkins_monitor = getJenkinsMonitor(job_name, job_build);
    if(jenkins_monitor){
        jenkins_monitor->StateChanged(job_state);
    }
}

void JobMonitor::gotJenkinsJobConsoleOutput(QString job_name, int job_build, QString, QString console_output){
    auto jenkins_monitor = getJenkinsMonitor(job_name, job_build);
    if(jenkins_monitor){
        jenkins_monitor->AppendLine(console_output);
    }
}

void JobMonitor::gotJenkinsJobArtifacts(QString job_name, int job_build, QString, QStringList artifacts){
    auto jenkins_monitor = getJenkinsMonitor(job_name, job_build);
    if(jenkins_monitor){
        jenkins_monitor->gotJobArtifacts(artifacts);
    }
}