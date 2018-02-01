#include "jenkinsmonitor.h"
#include "../../theme.h"
#include <QBoxLayout>

JenkinsMonitor::JenkinsMonitor(QString job_name, int build_number, QWidget * parent): Monitor(parent){
    this->job_name = job_name;
    this->build_number = build_number;

    setupLayout();
    themeChanged();
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &JenkinsMonitor::themeChanged);
    connect(this, &Monitor::StateChanged, this, &JenkinsMonitor::stateChanged);
    StateChanged(Notification::Severity::NONE);
}

void JenkinsMonitor::themeChanged(){
    auto theme = Theme::theme();

    toolbar->setIconSize(theme->getIconSize());
    
    setStyleSheet(theme->getWidgetStyleSheet("JenkinsMonitor"));
    text_browser->setStyleSheet(theme->getLineEditStyleSheet("QTextBrowser"));

    abort_action->setIcon(theme->getIcon("Icons", "circleCrossDark"));
    clear_action->setIcon(theme->getIcon("Icons", "bin"));
    close_action->setIcon(theme->getIcon("Icons", "cross"));
    
    url_action->setIcon(theme->getIcon("Icons", "globe"));

    toolbar->setIconSize(theme->getIconSize());
}

void JenkinsMonitor::stateChanged(Notification::Severity state){
    //Abortable
    abort_action->setEnabled(state == Notification::Severity::RUNNING);
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
    
    text_browser = new QTextBrowser(this);
    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    clear_action = toolbar->addAction("Clear");
    abort_action = toolbar->addAction("Abort Job");
    url_action = toolbar->addAction("Open Jenkins");
    close_action = toolbar->addAction("Close Tab");
    

    layout->addWidget(text_browser, 1);
    layout->addWidget(toolbar, 0, Qt::AlignRight);

    connect(this, &Monitor::AppendLine, text_browser, &QTextBrowser::append);
    connect(this, &Monitor::Clear, text_browser, &QTextBrowser::clear);

    connect(clear_action, &QAction::triggered, this, &Monitor::Clear);
    connect(abort_action, &QAction::triggered, this, &Monitor::Abort);
    connect(close_action, &QAction::triggered, this, &Monitor::Close);
    connect(url_action, &QAction::triggered, this, &JenkinsMonitor::GotoURL);
}