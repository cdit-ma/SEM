#include "welcomescreenwidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include "../../theme.h"
#include <QStringBuilder>
#include "../../Controllers/ActionController/actioncontroller.h"

#define MIN_WIDTH 800
#define MIN_HEIGHT 700


QToolButton* WelcomeScreenWidget::getButton(QAction* action){
    auto button = new QToolButton(this);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    //Connect the button to it's action so we don't need to worry about QToolButton stuff
    button->setDefaultAction(action);
    return button;
}

void WelcomeScreenWidget::refreshSize(){
    recent_project_toolbar->setFixedHeight(left_toolbar->height());
}

/**
 * @brief WelcomeScreenWidget::WelcomeScreenWidget
 * @param ac
 * @param parent
 */
WelcomeScreenWidget::WelcomeScreenWidget(ActionController* action_controller, QWidget *parent) : QWidget(parent)
{
    this->action_controller = action_controller;
    auto title_widget = new QWidget(this);
    auto title_layout = new QVBoxLayout(title_widget);
    medea_icon = new QLabel(this);
    medea_label = new QLabel("MEDEA");
    medea_version_label = new QLabel("Version " % APP_VERSION());

    title_layout->setSpacing(2);
    title_layout->addWidget(medea_icon, 0, Qt::AlignCenter);
    title_layout->addWidget(medea_label, 0, Qt::AlignCenter);
    title_layout->addWidget(medea_version_label, 0, Qt::AlignCenter);




    recent_project_label = new QToolButton(this);
    recent_project_label->setText("Recent Projects");
    recent_project_label->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    recent_project_label->setEnabled(false);

    left_toolbar = new QToolBar(this);
    left_toolbar->setOrientation(Qt::Vertical);
    left_toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    left_toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    left_toolbar->installEventFilter(this);

    left_toolbar->addWidget(title_widget);
    if (action_controller) {
        left_toolbar->addWidget(getButton(action_controller->file_newProject));
        left_toolbar->addWidget(getButton(action_controller->file_openProject));
        left_toolbar->addWidget(getButton(action_controller->options_settings));
    }

    
    bottom_toolbar = new QToolBar(this);
    bottom_toolbar->setOrientation(Qt::Horizontal);
    bottom_toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    
    if (action_controller) {
        bottom_toolbar->addAction(action_controller->help_wiki);
        bottom_toolbar->addAction(action_controller->help_aboutMedea);
        bottom_toolbar->addAction(action_controller->file_exit);
    }

    for(auto action : left_toolbar->actions()){
        auto widget = left_toolbar->widgetForAction(action);
        auto toolbutton = qobject_cast<QToolButton*>(widget);
        if(toolbutton){
            toolbutton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        }
    }


    recent_project_toolbar = new QToolBar(this);
    recent_project_toolbar->setOrientation(Qt::Vertical);
    recent_project_toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    recent_project_toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    recent_project_toolbar->setFixedHeight(400);
    
    auto right_widget = new QWidget(this);

    auto right_layout = new QVBoxLayout(right_widget);
    right_layout->setSpacing(2);
    right_layout->addWidget(recent_project_label);
    right_layout->addWidget(recent_project_toolbar, 1);

    auto h_layout = new QHBoxLayout();
    h_layout->addWidget(left_toolbar);
    h_layout->addWidget(right_widget, 1, Qt::AlignVCenter);

    auto main_widget = new QWidget(this);
    main_widget->setObjectName("WELCOME_WIDGET");
    main_widget->setMinimumWidth(MIN_WIDTH);

    auto main_layout = new QVBoxLayout(main_widget);
    main_layout->setSpacing(2);
    main_layout->addStretch();
    main_layout->addLayout(h_layout);
    main_layout->addWidget(bottom_toolbar, 0, Qt::AlignRight);
    main_layout->addStretch();


    auto layout = new QVBoxLayout(this);
    layout->addWidget(main_widget, 0, Qt::AlignCenter);

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));

    //connect(leftToolbar, SIGNAL(actionTriggered(QAction*)), this, SIGNAL(actionTriggered(QAction*)));
    //connect(recentProjectsToolbar, SIGNAL(actionTriggered(QAction*)), this, SIGNAL(actionTriggered(QAction*)));

    connect(action_controller, &ActionController::recentProjectsUpdated, this, &WelcomeScreenWidget::recentProjectsUpdated);

    setContextMenuPolicy(Qt::NoContextMenu);
    
    recentProjectsUpdated();
    themeChanged();
}


/**
 * @brief WelcomeScreenWidget::themeChanged
 */
void WelcomeScreenWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QToolButton{border: 0px;}"
                    "QToolButton:!hover{background: rgba(0,0,0,0); }");

    recent_project_label->setIcon(theme->getIcon("Icons", "clock"));
    recent_project_toolbar->setStyleSheet("QToolBar {"
                                         "background:" +theme->getAltBackgroundColorHex() + ";"
                                         "spacing: 0px;"
                                         "}"
                                         "QToolButton {"
                                         "border-radius: " + theme->getSharpCornerRadius() + ";"
                                         "border: 0px; }");
    
    
    auto icon_size = theme->getLargeIconSize();
    left_toolbar->setIconSize(icon_size);
    bottom_toolbar->setIconSize(icon_size);
    recent_project_toolbar->setIconSize(icon_size);
    recent_project_label->setIconSize(icon_size);

    medea_icon->setPixmap(theme->getImage("Icons", "medeaLogo", icon_size * 4));

    auto title_font = theme->getLargeFont();
    title_font.setPointSizeF(title_font.pointSizeF() * 3.0);

    medea_label->setFont(title_font);
    medea_label->setStyleSheet("color:" + theme->getTextColorHex() + ";");
    medea_version_label->setFont(theme->getLargeFont());
    medea_version_label->setStyleSheet("color:" + theme->getTextColorHex(Theme::CR_DISABLED) + ";");
}

bool WelcomeScreenWidget::eventFilter(QObject *object, QEvent *event)
{
    if (object && event->type() == QEvent::Resize) {
        refreshSize();
    }
    return false;
}



/**
 * @brief WelcomeScreenWidget::recentProjectsUpdated
 */
void WelcomeScreenWidget::recentProjectsUpdated()
{
    if (action_controller) {
        for(auto action : action_controller->getRecentProjectActions()){
            recent_project_toolbar->addWidget(getButton(action));
        }
    }
}

