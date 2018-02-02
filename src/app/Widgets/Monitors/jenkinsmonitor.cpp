#include "jenkinsmonitor.h"
#include "../../theme.h"
#include <QBoxLayout>
#include <QDesktopServices>
#include <QApplication>

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
    auto icon_size = theme->getIconSize();

    toolbar->setIconSize(icon_size);
    
    setStyleSheet(theme->getWidgetStyleSheet("JenkinsMonitor") + "QLabel {color:" + theme->getTextColorHex() + ";} ");
    text_browser->setStyleSheet(theme->getLineEditStyleSheet("QTextBrowser"));

    abort_action->setIcon(theme->getIcon("Icons", "circleCrossDark"));
    clear_action->setIcon(theme->getIcon("Icons", "bin"));
    close_action->setIcon(theme->getIcon("Icons", "cross"));
    
    url_action->setIcon(theme->getIcon("Icons", "globe"));

    toolbar->setIconSize(theme->getIconSize());

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
    abort_action->setEnabled(state == Notification::Severity::RUNNING);
    update_state_icon();
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
    icon_label = new QLabel(this);
    icon_label->setScaledContents(true);
    icon_label->setAlignment(Qt::AlignCenter);

    text_label->setText(job_name + " #" + QString::number(build_number));

    
    text_browser = new QTextBrowser(this);
    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    clear_action = toolbar->addAction("Clear");
    abort_action = toolbar->addAction("Abort Job");
    url_action = toolbar->addAction("Open Jenkins");
    close_action = toolbar->addAction("Close Tab");

       
    {
        auto h_layout = new QHBoxLayout();
        h_layout->setMargin(0);
        h_layout->setSpacing(5);

        h_layout->addWidget(icon_label);
        h_layout->addWidget(text_label, 1);
        layout->addLayout(h_layout, 0);
    }

    artifacts_box = new OptionGroupBox("Artifacts", this);
    artifacts_box->setExclusive(true);
    artifacts_box->setResetButtonVisible(false);
    artifacts_box->setCheckable(true);
    artifacts_box->setChecked(false);

    layout->addWidget(text_browser, 1);
    layout->addWidget(artifacts_box, 0);
    

    layout->addWidget(toolbar, 0, Qt::AlignRight);

    connect(this, &Monitor::AppendLine, text_browser, &QTextBrowser::append);
    connect(this, &Monitor::Clear, text_browser, &QTextBrowser::clear);

    connect(clear_action, &QAction::triggered, this, &Monitor::Clear);
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

void JenkinsMonitor::gotJobArtifacts(QStringList artifacts){
    for(auto artifact : artifacts){
        QUrl url(artifact);
        auto file_name = url.fileName();
        artifacts_box->addOption(artifact, file_name, "Icons", "package");
    }
    artifacts_box->setVisible(artifacts.size() > 0);
    if(artifacts.size() > 0){
        artifacts_box->setTitle("Artifacts [" + QString::number(artifacts.size()) + "]");
    }
}