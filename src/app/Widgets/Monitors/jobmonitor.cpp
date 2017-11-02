#include "jobmonitor.h"
#include "consolemonitor.h"
#include "jenkinsmonitor.h"
#include "../../theme.h"
#include <QBoxLayout>

JobMonitor::JobMonitor(QWidget *parent){
    setupLayout();

    connect(Theme::theme(), &Theme::theme_Changed, this, &JobMonitor::themeChanged);
    themeChanged();
}

QToolBar* JobMonitor::getToolbar(){
    return toolbar;
}

JenkinsMonitor* JobMonitor::constructJenkinsMonitor(QString job_name, int build_number){
    auto lookup_key = job_name + " #" + QString::number(build_number);
    if(!monitors.contains(lookup_key)){
        auto monitor = new JenkinsMonitor(job_name, build_number, this);
        tab_widget->addTab(monitor, lookup_key);
        monitors[lookup_key] = monitor;
        connect(monitor, &Monitor::StateChanged, this, &JobMonitor::MonitorStateChanged);
        return monitor;
    }
    return 0;
}

ConsoleMonitor* JobMonitor::constructConsoleMonitor(QString name){
    if(!monitors.contains(name)){
        auto monitor = new ConsoleMonitor(this);
        tab_widget->insertTab(0, monitor, name);
        monitors[name] = monitor;
        connect(monitor, &Monitor::StateChanged, this, &JobMonitor::MonitorStateChanged);
        return monitor;
    }
    return 0;
}

JenkinsMonitor* JobMonitor::getJenkinsMonitor(QString job_name, int build_number){
    auto lookup_key = job_name + " #" + QString::number(build_number);
    auto monitor = monitors.value(lookup_key, 0);
    return qobject_cast<JenkinsMonitor*>(monitor);
};

ConsoleMonitor* JobMonitor::getConsoleMonitor(QString name){
    auto monitor = monitors.value(name, 0);
    return qobject_cast<ConsoleMonitor*>(monitor);
};

void JobMonitor::updateTabIcon(int tab_id, Theme* theme){
    if(!theme){
        theme = Theme::theme();
    }
    auto monitor = qobject_cast<Monitor*>(tab_widget->widget(tab_id));
    if(monitor){
        auto state = monitor->getState();

        QString icon_path = "Icons";
        QString icon_name;
        switch(state){
            case Notification::Severity::ERROR:{
                icon_name = "sphereRed";
                break;
            }
            case Notification::Severity::INFO:{
                icon_name = "sphereGray";
                break;
            }
            case Notification::Severity::SUCCESS:{
                icon_name = "sphereBlue";
                break;
            }
            default:
                return;
        }
        tab_widget->setTabIcon(tab_id, theme->getIcon(icon_path, icon_name));
    }
}

void JobMonitor::MonitorStateChanged(Notification::Severity state){
    auto tab_id = getTab((QWidget*)sender());
    updateTabIcon(tab_id);
};

int JobMonitor::getTab(QWidget* widget){
    return tab_widget->indexOf(widget);
}

void JobMonitor::frameChanged(){
    if(loading_movie){
        auto icon = QIcon(loading_movie->currentPixmap());

        for(auto monitor : monitors){
            auto tab_id = getTab(monitor);

            if(monitor && monitor->getState() == Notification::Severity::RUNNING){
                tab_widget->setTabIcon(tab_id, icon);
            }
        }
    }
};

void JobMonitor::closeTab(int id){
    auto tab_name = tab_widget->tabText(id);

    auto monitor = monitors.value(tab_name, 0);
    if(monitor){
        monitors.remove(tab_name);
        tab_widget->removeTab(id);

        delete monitor;
    }

    //Hide!
    if(parentWidget() && tab_widget->count() == 0){
        parentWidget()->hide();
    }
};

void JobMonitor::setupLayout(){
    auto layout = new QVBoxLayout(this);
    layout->setMargin(5);
    layout->setSpacing(2);

    tab_widget = new QTabWidget(this);
    tab_widget->setContentsMargins(QMargins(5,5,5,5));

    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    
    layout->addWidget(tab_widget, 1);
    layout->addWidget(toolbar, 0, Qt::AlignRight);

    loading_movie = Theme::theme()->getGif("Icons", "loading");
    connect(loading_movie, &QMovie::frameChanged, this, &JobMonitor::frameChanged);

    tab_widget->setTabsClosable(true);
    connect(tab_widget, &QTabWidget::tabCloseRequested, this, &JobMonitor::closeTab);

    tab_widget->setElideMode(Qt::ElideMiddle);
};

void JobMonitor::themeChanged(){
    auto theme = Theme::theme();
    setStyleSheet(theme->getWidgetStyleSheet("JobMonitor"));
    tab_widget->setStyleSheet(theme->getTabbedWidgetStyleSheet() +
                             "QTabBar::tab:selected{ background:" % theme->getPressedColorHex() % "; color:" % theme->getTextColorHex(ColorRole::SELECTED) % ";}"
                             "QTabBar::tab:hover{ background:" % theme->getHighlightColorHex() % ";}");
    toolbar->setIconSize(theme->getIconSize());

    for(auto i = 0 ; i < tab_widget->count(); i++){
        updateTabIcon(i, theme);
    }
};