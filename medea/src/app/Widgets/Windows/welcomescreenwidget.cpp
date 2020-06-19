#include "welcomescreenwidget.h"
#include "../../theme.h"
#include "../../Controllers/ActionController/actioncontroller.h"
#include "../../../modelcontroller/version.h"

#include <QVBoxLayout>
#include <QStringBuilder>

#define MIN_WIDTH 800

QToolButton* WelcomeScreenWidget::getButton(QAction* action, Qt::ToolButtonStyle style)
{
    auto button = new QToolButton(this);
    button->setToolButtonStyle(style);
    button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    //Connect the button to it's action so we don't need to worry about QToolButton stuff
    button->setDefaultAction(action);
    return button;
}

/**
 * @brief WelcomeScreenWidget::WelcomeScreenWidget
 * @param ac
 * @param parent
 */
WelcomeScreenWidget::WelcomeScreenWidget(ActionController* action_controller, QWidget *parent)
    : QWidget(parent)
{
    this->action_controller = action_controller;

    auto title_layout = new QVBoxLayout();
    medea_icon = new QLabel(this);
    medea_label = new QLabel("MEDEA");
    medea_version_label = new QLabel("Version " % Version::GetMedeaVersion());

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

    if (action_controller) {
        left_toolbar->addWidget(getButton(action_controller->file_newProject, Qt::ToolButtonTextUnderIcon));
        left_toolbar->addWidget(getButton(action_controller->file_openProject, Qt::ToolButtonTextUnderIcon));
        left_toolbar->addWidget(getButton(action_controller->options_settings, Qt::ToolButtonTextUnderIcon));
        left_toolbar->addWidget(getButton(action_controller->file_exit, Qt::ToolButtonTextUnderIcon));
    }

    bottom_toolbar = new QToolBar(this);
    bottom_toolbar->setOrientation(Qt::Horizontal);
    
    if (action_controller) {
        bottom_toolbar->addWidget(getButton(action_controller->help_wiki, Qt::ToolButtonTextUnderIcon));
        bottom_toolbar->addWidget(getButton(action_controller->help_aboutMedea, Qt::ToolButtonTextUnderIcon));
    }

    for (auto action : left_toolbar->actions()) {
        auto widget = left_toolbar->widgetForAction(action);
        auto toolbutton = qobject_cast<QToolButton*>(widget);
        if (toolbutton) {
            toolbutton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        }
    }

    recent_project_toolbar = new QToolBar(this);
    recent_project_toolbar->setOrientation(Qt::Vertical);
    recent_project_toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    recent_project_toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto left_layout = new QVBoxLayout();
    left_layout->setSpacing(10);
    left_layout->addLayout(title_layout);
    left_layout->addWidget(left_toolbar);

    auto right_layout = new QVBoxLayout();
    right_layout->setSpacing(2);
    right_layout->addWidget(recent_project_label);
    right_layout->addWidget(recent_project_toolbar, 1);

    auto h_layout = new QHBoxLayout();
    h_layout->setSpacing(10);
    h_layout->addLayout(left_layout, 0);
    h_layout->addLayout(right_layout, 1);

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

    connect(Theme::theme(), &Theme::theme_Changed, this, &WelcomeScreenWidget::themeChanged);
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

    auto icon_size = theme->getLargeIconSize();
    recent_project_toolbar->setIconSize(icon_size);
    recent_project_toolbar->setStyleSheet(theme->getToolBarStyleSheet() +
                                          "QToolBar {"
                                          "background:" + theme->getAltBackgroundColorHex() + ";"
                                          "}"
                                          "QToolButton {"
                                          "border-radius: " + theme->getSharpCornerRadius() + ";"
                                          "border: 0px; }");
    
    const auto& toolbar_style = "QToolButton{ border: 0px; }"
                                "QToolButton:!hover{ background: rgba(0,0,0,0); }";

    left_toolbar->setIconSize(icon_size);
    left_toolbar->setStyleSheet(theme->getToolBarStyleSheet() + toolbar_style);

    bottom_toolbar->setIconSize(icon_size);
    bottom_toolbar->setStyleSheet(theme->getToolBarStyleSheet() + toolbar_style);

    recent_project_label->setIconSize(icon_size);
    recent_project_label->setIcon(theme->getIcon("Icons", "clock"));
    recent_project_label->setStyleSheet("color:" + theme->getTextColorHex() + ";");

    medea_icon->setPixmap(theme->getImage("Icons", "medeaLogo", icon_size * 4));

    auto title_font = theme->getLargeFont();
    title_font.setPointSizeF(title_font.pointSizeF() * 3.0);

    medea_label->setFont(title_font);
    medea_label->setStyleSheet("color:" + theme->getTextColorHex() + ";");
    medea_version_label->setFont(theme->getLargeFont());
    medea_version_label->setStyleSheet("color:" + theme->getTextColorHex(ColorRole::DISABLED) + ";");
}

/**
 * @brief WelcomeScreenWidget::recentProjectsUpdated
 */
void WelcomeScreenWidget::recentProjectsUpdated()
{
    if (action_controller) {
        recent_project_toolbar->clear();
        for (auto action : action_controller->getRecentProjectActions()) {
            recent_project_toolbar->addWidget(getButton(action));
        }
    }
}