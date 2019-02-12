#include "jenkinsmonitor.h"
#include "../../theme.h"


#include <QBoxLayout>
#include <QDesktopServices>
#include <QApplication>
#include <QJsonArray>

JenkinsMonitor::JenkinsMonitor(JenkinsManager* jenkins_manager, QString job_name, int build_number, QWidget * parent): Monitor(parent){
    this->job_name = job_name;
    this->build_number = build_number;
    this->jenkins_manager = jenkins_manager;

    setupLayout();
    themeChanged();
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &JenkinsMonitor::themeChanged);
    connect(this, &Monitor::StateChanged, this, &JenkinsMonitor::stateChanged);

    console_watcher = new QFutureWatcher<QString>(this);
    status_watcher = new QFutureWatcher<Jenkins_Job_Status>(this);
    artifact_watcher = new QFutureWatcher<QJsonDocument>(this);
    
    connect(console_watcher, &QFutureWatcher<QString>::finished, this, &JenkinsMonitor::ConsoleUpdated);
    connect(status_watcher, &QFutureWatcher<Jenkins_Job_Status>::finished, this, &JenkinsMonitor::StatusUpdated);
    connect(artifact_watcher, &QFutureWatcher<QJsonDocument>::finished, this, &JenkinsMonitor::JobFinished);
    
    
    refresh_timer = new QTimer(this);
    refresh_timer->setSingleShot(true);
    connect(refresh_timer, &QTimer::timeout, this, &JenkinsMonitor::Refresh);
    Refresh();
}

JenkinsMonitor::~JenkinsMonitor(){
//    refresh_timer->stop();
}

void JenkinsMonitor::ConsoleUpdated(){
    auto current_text = text_browser->toPlainText();
    auto new_text = console_watcher->result();
    if(new_text.size() > current_text.size()){
        //Emit the live difference
        QString delta_data = new_text.mid(current_text.size());
        text_browser->moveCursor(QTextCursor::End);
        text_browser->insertPlainText(delta_data);
        text_browser->moveCursor(QTextCursor::End);
    }
    Refresh();
}

void JenkinsMonitor::JobFinished(){
    //Get the Json document
    auto json = artifact_watcher->result();

    if(!json.isNull()){
        auto config = json.object();

        for(auto artifact : config["artifacts"].toArray()){
            auto relative_path = artifact.toObject()["relativePath"].toString();
            auto url_str = jenkins_manager->GetArtifactUrl(job_name, build_number, relative_path);
            QUrl url(url_str);
            artifacts_box->addOption(url_str, url.fileName(), "Icons", "package");
        }
        auto option_size = artifacts_box->getOptionCount();
        artifacts_box->setVisible(option_size > 0);
        if(option_size > 0){
            artifacts_box->setTitle("Artifacts [" + QString::number(option_size) + "]");
        }
    }
}



void JenkinsMonitor::Refresh(){
    //If we haven't got a timer started, and our state is RUNNING request
    if(!refresh_timer->isActive() && getState() == Notification::Severity::RUNNING){
        refresh_timer->start(500);
        auto job_status = jenkins_manager->GetJobStatus(job_name, build_number);
        auto job_console = jenkins_manager->GetJobConsoleOutput(job_name, build_number);

        console_watcher->setFuture(job_console.second);
        status_watcher->setFuture(job_status.second);
    }
}

void JenkinsMonitor::StatusUpdated(){
    auto result = status_watcher->result();

    StateChanged(result.state);
    SetDuration(result.current_duration);
    SetUser(result.user_id);
    SetDescription(result.description);

    Refresh();
}

void JenkinsMonitor::SetDuration(int current_duration){
    auto got_valid = current_duration > 0;
    if(got_valid){
        auto time_str = QDateTime::fromTime_t(current_duration/ 1000.0).toUTC().toString("hh:mm:ss");
        duration_label->setText(time_str);
    }
    //Hide and show based on duration
    duration_label->setVisible(got_valid);
    duration_icon_label->setVisible(got_valid);
}
void JenkinsMonitor::SetUser(QString user){
    auto got_valid = user != "";
    if(got_valid){
        user_label->setText(user);
    }
    user_label->setVisible(got_valid);
    user_icon_label->setVisible(got_valid);
}

void JenkinsMonitor::SetDescription(QString description){
    auto got_valid = description != "";
    if(got_valid){
        description_label->setText(description);
    }
    description_label->setVisible(got_valid);
    description_icon_label->setVisible(got_valid);
}

void JenkinsMonitor::themeChanged(){
    auto theme = Theme::theme();
    auto icon_size = theme->getIconSize();

    bottom_toolbar->setIconSize(icon_size);
    top_toolbar->setIconSize(icon_size);
    
    setStyleSheet(theme->getWidgetStyleSheet("JenkinsMonitor") + "QLabel {color:" + theme->getTextColorHex() + ";} ");
    text_browser->setStyleSheet(theme->getLineEditStyleSheet("QTextBrowser"));

    abort_action->setIcon(theme->getIcon("Icons", "circleCrossDark"));
    close_action->setIcon(theme->getIcon("Icons", "cross"));

    text_label->setStyleSheet("font-weight: bold;");
    
    url_action->setIcon(theme->getIcon("Icons", "globe"));

    {
        auto pixmap = theme->getImage("Icons", "clock", icon_size, theme->getTextColor());
        duration_icon_label->setFixedSize(icon_size);
        duration_icon_label->setPixmap(pixmap);

    }
    {
        auto pixmap = theme->getImage("Icons", "person", icon_size, theme->getTextColor());
        user_icon_label->setFixedSize(icon_size);
        user_icon_label->setPixmap(pixmap);
    }
    {
        auto pixmap = theme->getImage("Icons", "label", icon_size, theme->getTextColor());
        description_icon_label->setFixedSize(icon_size);
        description_icon_label->setPixmap(pixmap);
    }
    update_state_icon();
}

void JenkinsMonitor::update_state_icon(){
    auto theme = Theme::theme();
    auto icon_size = theme->getLargeIconSize();
    auto state = getState();

    if(state == Notification::Severity::RUNNING){
        auto movie = Theme::theme()->getGif("Icons", "loading");
        icon_label->setMovie(movie);
    }else{
        auto pixmap = theme->getImage("Jenkins", Notification::getSeverityString(state), icon_size);
        icon_label->setFixedSize(icon_size);
        
        if(pixmap.isNull()){
            pixmap = theme->getImage("Icons", "Help", icon_size);
        }
        icon_label->setPixmap(pixmap);
    }
}


void JenkinsMonitor::stateChanged(Notification::Severity state){
    //Abortable
    //abort_action->setEnabled(state == Notification::Severity::RUNNING);
    update_state_icon();

    //If our state is finished, get the list of artifacts
    if(state != Notification::Severity::RUNNING){
        auto job_config = jenkins_manager->GetJobConfiguration(job_name, build_number);
        artifact_watcher->setFuture(job_config.second);
    }
}

int JenkinsMonitor::getBuildNumber() const{
    return this->build_number;
}

QString JenkinsMonitor::GetJobName() const{
    return this->job_name;
}

void JenkinsMonitor::setupLayout(){
    auto layout = new QVBoxLayout(this);
    layout->setMargin(2);
    layout->setSpacing(2);
    

    text_label = new QLabel(this);
    text_label->setText(job_name + " #" + QString::number(build_number));

    icon_label = new QLabel(this);
    icon_label->setScaledContents(true);
    icon_label->setAlignment(Qt::AlignCenter);

    duration_label = new QLabel(this);
    user_label = new QLabel(this);
    description_label = new QLabel(this);
    
    duration_icon_label = new QLabel(this);
    duration_icon_label->setScaledContents(true);
    duration_icon_label->setAlignment(Qt::AlignCenter);

    user_icon_label = new QLabel(this);
    user_icon_label->setScaledContents(true);
    user_icon_label->setAlignment(Qt::AlignCenter);

    description_icon_label = new QLabel(this);
    description_icon_label->setScaledContents(true);
    description_icon_label->setAlignment(Qt::AlignCenter);

    

    

    user_label->hide();
    duration_label->hide();
    description_label->hide();

    user_icon_label->hide();
    duration_icon_label->hide();
    description_icon_label->hide();
    
    text_browser = new QTextBrowser(this);
    text_browser->setReadOnly(true);
    top_toolbar = new QToolBar(this);
    bottom_toolbar = new QToolBar(this);
    bottom_toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    
    url_action = bottom_toolbar->addAction("Open Jenkins");
    abort_action = bottom_toolbar->addAction("Abort Job");
    abort_action->setEnabled(false);
    close_action = top_toolbar->addAction("Close Tab");

       
    {
        auto h_layout = new QGridLayout();
        h_layout->setMargin(0);
        h_layout->setSpacing(5);

        h_layout->addWidget(icon_label,             0, 0, 2, 2, Qt::AlignCenter);
        h_layout->addWidget(text_label,             0, 2, 1, 2, Qt::AlignLeft);

        h_layout->addWidget(user_label,             0, 4, 1, 1, Qt::AlignRight);
        h_layout->addWidget(user_icon_label,        0, 5, 1, 1, Qt::AlignCenter);

        h_layout->addWidget(description_icon_label, 1, 2, 1, 1, Qt::AlignCenter);
        h_layout->addWidget(description_label,      1, 3, 1, 1, Qt::AlignLeft);

        h_layout->addWidget(duration_label,         1, 4, 1, 1, Qt::AlignRight);
        h_layout->addWidget(duration_icon_label,    1, 5, 1, 1, Qt::AlignCenter);

        h_layout->addWidget(top_toolbar,            0, 6, 2, 2, Qt::AlignCenter);

        h_layout->setColumnStretch(3, 1);
        h_layout->setColumnStretch(4, 1);

        layout->addLayout(h_layout);
    }

    artifacts_box = new OptionGroupBox("Artifacts", SortOrder::INSERTION, this);
    artifacts_box->setExclusive(true);
    artifacts_box->setResetButtonVisible(false);
    artifacts_box->setCheckable(true);
    artifacts_box->setChecked(false);
    artifacts_box->hide();

    layout->addWidget(text_browser, 1);
    layout->addWidget(artifacts_box, 0);
    

    layout->addWidget(bottom_toolbar, 0, Qt::AlignRight);

    connect(abort_action, &QAction::triggered, this, &Monitor::Abort);
    connect(close_action, &QAction::triggered, this, &Monitor::Close);
    connect(url_action, &QAction::triggered, this, &JenkinsMonitor::GotoURL);

    connect(artifacts_box, &OptionGroupBox::checkedOptionsChanged, this, &JenkinsMonitor::artifactPressed);
}

void JenkinsMonitor::artifactPressed(){
    QString artifact_url;

    if(artifacts_box){
        auto artifact_selected = artifacts_box->getCheckedOptions<QString>().toSet();

        if(!artifacts_box->isResetChecked()){
            artifact_url = *artifact_selected.constBegin();
        }
    }
    if(artifact_url.size() > 0){
        //Open the url
        QDesktopServices::openUrl(QUrl(artifact_url));
        artifacts_box->reset();
    }
}
