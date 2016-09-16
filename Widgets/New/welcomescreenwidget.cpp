#include "welcomescreenwidget.h"

#include <QLabel>
#include <QVBoxLayout>


/**
 * @brief WelcomeScreenWidget::WelcomeScreenWidget
 * @param ac
 * @param parent
 */
WelcomeScreenWidget::WelcomeScreenWidget(ActionController* ac, QWidget *parent) : QWidget(parent)
{
    actionController = ac;

    QLabel* medeaIcon = new QLabel(this);
    QLabel* medeaLabel = new QLabel("MEDEA");
    QLabel* medeaVersionLabel = new QLabel("Version 2.0"); // + MEDEA_VERSION);
    medeaLabel->setStyleSheet("font-size: 32pt; color: white; text-align: center;");
    medeaVersionLabel->setStyleSheet("font-size: 12pt; color: gray; text-align: center;");

    QPixmap pixMap = Theme::theme()->getImage("Actions", "MEDEA");
    pixMap = pixMap.scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    medeaIcon->setPixmap(pixMap);

    QWidget* leftTopWidget = new QWidget(this);
    QVBoxLayout* leftTopLayout = new QVBoxLayout(leftTopWidget);
    leftTopLayout->setSpacing(2);
    leftTopLayout->addWidget(medeaIcon, 0, Qt::AlignCenter);
    leftTopLayout->addWidget(medeaLabel, 0, Qt::AlignCenter);
    leftTopLayout->addWidget(medeaVersionLabel, 0, Qt::AlignCenter);

    QSize iconSize(24,24);

    recentProjectsLabel = new QToolButton(this);
    recentProjectsLabel->setText("Recent Projects");
    recentProjectsLabel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    recentProjectsLabel->setIcon(Theme::theme()->getIcon("Welcome", "Timer"));
    recentProjectsLabel->setIconSize(iconSize);
    recentProjectsLabel->setEnabled(false);

    QToolBar* leftToolbar = new QToolBar(this);
    leftToolbar->setIconSize(iconSize);
    leftToolbar->setOrientation(Qt::Vertical);
    leftToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    leftToolbar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    leftToolbar->setStyleSheet("QToolButton{ width: 110px; }");

    QToolBar* bottomToolbar = new QToolBar(this);
    bottomToolbar->setIconSize(iconSize);
    bottomToolbar->setOrientation(Qt::Horizontal);
    bottomToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    bottomToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    bottomToolbar->setLayoutDirection(Qt::RightToLeft);
    bottomToolbar->setStyleSheet("QToolButton{ padding-left: 10px; }");

    recentProjectsToolbar = new QToolBar(this);
    recentProjectsToolbar->setIconSize(QSize(18, 18));
    recentProjectsToolbar->setOrientation(Qt::Vertical);
    recentProjectsToolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    recentProjectsToolbar->setFixedSize(400, 350);

    if (ac) {
        leftToolbar->addWidget(leftTopWidget);
        leftToolbar->addAction(ac->file_newProject);
        leftToolbar->addAction(ac->file_openProject);
        leftToolbar->addAction(ac->options_settings);
    }

    bottomToolbar->addAction(ac->file_exit);
    bottomToolbar->addAction(ac->help_aboutMedea);
    bottomToolbar->addAction(ac->help_wiki);

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addStretch();
    vLayout->addSpacerItem(new QSpacerItem(0, 15));
    vLayout->addWidget(recentProjectsLabel);
    vLayout->addWidget(recentProjectsToolbar, 0, Qt::AlignLeft | Qt::AlignBottom);
    vLayout->addWidget(bottomToolbar, 0, Qt::AlignLeft | Qt::AlignTop);
    vLayout->addStretch();

    QWidget* mainWidget = new QWidget();
    mainWidget->setObjectName("WELCOME_WIDGET");

    QHBoxLayout* containerLayout = new QHBoxLayout(mainWidget);
    containerLayout->addStretch();
    containerLayout->addWidget(leftToolbar, 0, Qt::AlignVCenter | Qt::AlignRight);
    containerLayout->addLayout(vLayout);
    containerLayout->addStretch();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(mainWidget);

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(leftToolbar, SIGNAL(actionTriggered(QAction*)), this, SIGNAL(actionTriggered(QAction*)));
    connect(recentProjectsToolbar, SIGNAL(actionTriggered(QAction*)), this, SIGNAL(actionTriggered(QAction*)));

    recentProjectsUpdated();
    themeChanged();
}


/**
 * @brief WelcomeScreenWidget::themeChanged
 */
void WelcomeScreenWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QWidget#WELCOME_WIDGET{ background:" + theme->getBackgroundColorHex() + ";}"
                  "QToolButton:!hover{ border: 0px; background: rgba(0,0,0,0); }");

    recentProjectsLabel->setIcon(theme->getIcon("Actions", "Timer"));
    recentProjectsToolbar->setStyleSheet("QToolBar {"
                                         "background:" +theme->getAltBackgroundColorHex() + ";"
                                         "spacing: 0px;"
                                         "}"
                                         "QToolButton {"
                                         "width: 400px;"
                                         "border-radius: 2px;"
                                         "border: 0px; }");
}


/**
 * @brief WelcomeScreenWidget::recentProjectsUpdated
 */
void WelcomeScreenWidget::recentProjectsUpdated()
{
    if (actionController) {
        foreach(RootAction* action, actionController->getRecentProjectActions()){
            recentProjectsToolbar->addAction(action);
        }
    }
}

