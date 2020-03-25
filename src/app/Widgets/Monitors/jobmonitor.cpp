#include "jobmonitor.h"
#include "consolemonitor.h"
#include "jenkinsmonitor.h"
#include "../../theme.h"

#include <QFutureWatcher>
#include <QJsonArray>
#include <QJsonObject>

JobMonitor::JobMonitor(JenkinsManager* jenkins_manager, QWidget* parent)
{
    this->jenkins_manager = jenkins_manager;
    setupLayout();

    auto theme = Theme::theme();
    connect(theme, &Theme::theme_Changed, this, &JobMonitor::themeChanged);

    //Set Icon Alias
    theme->setIconAlias("Jenkins", Notification::getSeverityString(Notification::Severity::ERROR), "Icons", "sphereRed");
    theme->setIconAlias("Jenkins", Notification::getSeverityString(Notification::Severity::INFO), "Icons", "sphereGray");
    theme->setIconAlias("Jenkins", Notification::getSeverityString(Notification::Severity::SUCCESS), "Icons", "sphereBlue");
    theme->setIconAlias("Jenkins", Notification::getSeverityString(Notification::Severity::RUNNING), "Icons", "running");
    theme->setIconAlias("Jenkins", Notification::getSeverityString(Notification::Severity::NONE), "Icons", "sphereGray");

    //Connect a job being queued with the refresh signal
    connect(jenkins_manager, &JenkinsManager::JobQueued, this, &JobMonitor::refreshRecentBuildsByName);

    themeChanged();
    refreshRecentBuilds();
}

QToolBar* JobMonitor::getToolbar()
{
    return toolbar;
}

JenkinsMonitor* JobMonitor::constructJenkinsMonitor(const QString& job_name, int build_number)
{
    auto lookup_key = getJobKey(job_name, build_number);
    if(!monitors.contains(lookup_key)){
        auto monitor = new JenkinsMonitor(jenkins_manager, job_name, build_number, this);
        stacked_widget->addWidget(monitor);
        monitors[lookup_key] = monitor;
        connect(monitor, &Monitor::StateChanged, this, &JobMonitor::MonitorStateChanged);
        connect(monitor, &Monitor::Close, this, &JobMonitor::MonitorClose);
        return monitor;
    }
    // INSPECT: (Ask Jackson) - Shouldn't this return monitors.value(lookup_key)?
    return nullptr;
}

ConsoleMonitor* JobMonitor::constructConsoleMonitor()
{
    QString name = "Local Deployment";
    auto lookup_key = getJobKey(name, 0);
    if(!monitors.contains(lookup_key)){
        if(!running_jobs_box->gotOption(lookup_key)){
            running_jobs_box->addOption(lookup_key, name, "Icons", "circleQuestion");
        }
        auto monitor = new ConsoleMonitor(this);
        stacked_widget->addWidget(monitor);
        monitors[lookup_key] = monitor;
        connect(monitor, &Monitor::StateChanged, this, &JobMonitor::MonitorStateChanged);
        return monitor;
    }
    // INSPECT: (Ask Jackson) - Shouldn't this return monitors.value(lookup_key)?
    return nullptr;
}

Monitor* JobMonitor::getMonitor(const QString& job_name, int build_number)
{
    auto lookup_key = getJobKey(job_name, build_number);
    return monitors.value(lookup_key, nullptr);
};

ConsoleMonitor* JobMonitor::getConsoleMonitor(const QString& name)
{
    return qobject_cast<ConsoleMonitor*>(getMonitor(name, 0));
};

void JobMonitor::updateMonitorIcon(Monitor* monitor, Theme* theme)
{
    Q_UNUSED(theme);
    if (monitor) {
        auto jenkins_monitor = qobject_cast<JenkinsMonitor*>(monitor);
        auto state = monitor->getState();

        int job_id = -1;
        QString job_name;

        if (jenkins_monitor) {
            job_id = jenkins_monitor->getBuildNumber();
            job_name = jenkins_monitor->GetJobName();
        } else {
            job_id = 0;
            job_name = "Local Deployment";
        }
        updateMonitorIcon(job_name, job_id, state);
    }
}

void JobMonitor::updateMonitorIcon(const QString& job_name, int job_number, Notification::Severity state)
{
    auto key = getJobKey(job_name, job_number);
    running_jobs_box->updateOptionIcon(key, "Jenkins", Notification::getSeverityString(state));
}

void JobMonitor::MonitorStateChanged(Notification::Severity state)
{
    auto monitor = qobject_cast<Monitor*>(sender());
    updateMonitorIcon(monitor);
}

void JobMonitor::MonitorClose()
{
    auto monitor = qobject_cast<Monitor*>(sender());
    if (monitor) {
        auto&& key = monitors.key(monitor);
        monitors.remove(key);
        stacked_widget->removeWidget(monitor);
        monitor->deleteLater();
    }
}

void JobMonitor::stackedWidgetChanged(int index)
{
    auto widget = stacked_widget->widget(index);
    auto monitor = qobject_cast<Monitor*>(widget);
    if(monitor){
        auto key = monitors.key(monitor);
        running_jobs_box->setOptionChecked(key, true);
    }else{
        running_jobs_box->resetChecked(false);
    }
}

void JobMonitor::setupLayout()
{
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
    splitter->setSizes({250, 350});

    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(splitter, 1);

    {
        running_jobs_box = new OptionGroupBox("Jobs", SortOrder::DESCENDING, this);
        running_jobs_box->setExclusive(true);
        running_jobs_box->setCheckable(false);
        running_jobs_box->setResetButtonIcon("Icons", "refresh");
        running_jobs_box->setResetButtonText("Refresh Jenkins Jobs List");
        jobs_list_layout->addWidget(running_jobs_box);

        connect(running_jobs_box, &OptionGroupBox::checkedOptionsChanged, this, &JobMonitor::jobPressed);
        connect(running_jobs_box, &OptionGroupBox::resetPressed, this, &JobMonitor::refreshRecentBuilds);
    }
};

QPair<QString, int> JobMonitor::splitJobKey(const QString& key)
{
    int last_index = key.lastIndexOf("#");
    auto left = key.left(last_index);
    auto right = key.mid(last_index + 1);
    bool ok;
    int id = right.toInt(&ok);
    if(!ok){
        id = -1;
    }
    return {left, id};
}   

QString JobMonitor::getJobKey(const QString& job_name, int job_number)
{
    return job_name + "#" + QString::number(job_number);
}

void JobMonitor::refreshRecentBuilds()
{
    if(jenkins_manager){
        refreshRecentBuildsByName(jenkins_manager->GetJobName());
    }
}

void JobMonitor::refreshRecentBuildsByName(const QString& job_name)
{
    if(jenkins_manager){
        auto request_config = jenkins_manager->GetJobConfiguration(job_name);
        auto future_watcher = new QFutureWatcher<QJsonDocument>(this);
        connect(future_watcher, &QFutureWatcher<QJsonDocument>::finished, [=](){
            gotJobConfig(future_watcher->result());
        });
        future_watcher->setFuture(request_config.second);
    }
}

void JobMonitor::gotJobConfig(const QJsonDocument& document)
{
    auto json = document.object();
    auto job_name = json["name"].toString();

    //Only take the first 10 jobs
    auto request_count = 10;

    for (const auto& job : json["builds"].toArray()) {
        auto job_number = job.toObject()["number"].toInt(-1);
        if (job_number > 0 && (--request_count >= 0)) {
            auto job_watcher = new QFutureWatcher<Jenkins_Job_Status>(this);
            auto job_future = jenkins_manager->GetJobStatus(job_name, job_number);
            connect(job_watcher, &QFutureWatcher<Jenkins_Job_Status>::finished, [=](){
                JobStatusChanged(job_watcher->result());
            });
            job_watcher->setFuture(job_future.second);
        }
    }
}

void JobMonitor::JobStatusChanged(const Jenkins_Job_Status& job_status)
{
    auto users_match = jenkins_manager->GetUser() == job_status.user_id;
    auto request_user_jobs = SettingsController::settings()->getSetting(SETTINGS::JENKINS_REQUEST_USER_JOBS).toBool();
    if (users_match || !request_user_jobs) {
        auto key = getJobKey(job_status.name, job_status.number);
        auto title = job_status.name + " #" + QString::number(job_status.number);
        if (!running_jobs_box->gotOption(key)) {
            running_jobs_box->addOption(key, title, "Icons", "circleQuestion");
        } else {
            running_jobs_box->updateOptionLabel(key, title);
        }
        updateMonitorIcon(job_status.name, job_status.number, job_status.state);
    }
}

void JobMonitor::jobPressed()
{
    QString job_key;
    if (running_jobs_box) {
        // TODO: Ask Jackson if we could just leave this as a list since it also has a constBegin()
        //  If not, use an alternative way to cast it to a set; toSet() is deprecated
        auto jobs_selected = running_jobs_box->getCheckedOptions<QString>().toSet();
        if (!running_jobs_box->isResetChecked()) {
            job_key = *jobs_selected.constBegin();
        }
    }
    auto split_key = splitJobKey(job_key);
    if (split_key.second >= 0) {
        requestJobConsoleOutput(split_key.first, split_key.second);
    }
}

void JobMonitor::requestJobConsoleOutput(const QString& job_name, int job_id)
{
    auto job_monitor = getMonitor(job_name, job_id);
    if (!job_monitor) {
        //Has to be Jenkins
        auto jenkins_job_monitor = constructJenkinsMonitor(job_name, job_id);
        // NOTE: If the function above returned the existing monitor if we have one, we don't need null checks here and below
        if (jenkins_job_monitor && jenkins_manager) {
            connect(jenkins_job_monitor, &JenkinsMonitor::Abort, [=](){ jenkins_manager->AbortJob(job_name, job_id); });
            connect(jenkins_job_monitor, &JenkinsMonitor::GotoURL, [=](){ jenkins_manager->GotoJob(job_name, job_id); });
            job_monitor = jenkins_job_monitor;
        }
    }
    if (job_monitor) {
        stacked_widget->setCurrentWidget(job_monitor);
    }
}

void JobMonitor::themeChanged()
{
    auto theme = Theme::theme();
    setStyleSheet("JobMonitor{ background-color:" % theme->getBackgroundColorHex() % "; border:1px solid " % theme->getDisabledBackgroundColorHex() % ";}" %
                  "QScrollArea{ border: 1px solid " % theme->getAltBackgroundColorHex() % "; background: rgba(0,0,0,0); } " %
                  "QLabel{ color:" % theme->getTextColorHex() + ";} " %
                  theme->getToolBarStyleSheet() %
                  theme->getSplitterStyleSheet() %
                  "QToolButton::checked:!hover{ background: " % theme->getAltBackgroundColorHex() % ";}"
    );

    toolbar->setIconSize(theme->getIconSize());
    jobs_list_widget->setStyleSheet("QWidget#BLANK_WIDGET{background: rgba(0,0,0,0);}");

    for (const auto& monitor : monitors.values()) {
        updateMonitorIcon(monitor);
    }
};