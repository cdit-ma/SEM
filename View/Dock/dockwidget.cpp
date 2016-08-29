#include "dockwidget.h"
#include "../theme.h"
#include <QDebug>

/**
 * @brief DockWidget::DockWidget
 * @param parent
 */
DockWidget::DockWidget(ToolActionController* tc, ToolActionController::DOCK_TYPE type, QWidget *parent) : QScrollArea(parent)
{
    toolActionController = tc;
    dockType = type;

    switch (dockType) {
    case ToolActionController::DEFINITIONS:
    case ToolActionController::FUNCTIONS:
        containsHeader = true;
        break;
    default:
        containsHeader = false;
        break;
    }

    mainLayout = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
    mainLayout->setSpacing(5);

    alignLayout = new QVBoxLayout();
    alignLayout->addLayout(mainLayout);
    alignLayout->addStretch();

    mainWidget = new QWidget(this);
    mainWidget->setObjectName("DOCKWIDGET_MAIN");
    mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainWidget->setLayout(alignLayout);

    setWidget(mainWidget);
    setWidgetResizable(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setupHeaderLayout();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}


/**
 * @brief DockWidget::addAction
 * @param action
 * @return
 */
DockActionWidget* DockWidget::addAction(QAction* action)
{
    DockActionWidget* dockAction = 0;
    if (action && !childrenActions.contains(action)) {
        dockAction = new DockActionWidget(action, this);
        dockAction->setProperty("kind", action->text());

        if (toolActionController->kindsWithSubActions.contains(action->text())) {
            dockAction->setSubActionRequired(true);
        }

        childrenActions[action] = dockAction;
        mainLayout->addWidget(dockAction, 0, Qt::AlignTop);
        connect(dockAction, SIGNAL(clicked(bool)), this, SLOT(dockActionClicked()));
    }
    return dockAction;
}


/**
 * @brief DockWidget::addActions
 * @param actions
 */
void DockWidget::addActions(QList<QAction*> actions)
{
    foreach (QAction* action, actions) {
        addAction(action);
    }
}


/**
 * @brief DockWidget::clearDock
 */
void DockWidget::clearDock()
{
    foreach (DockActionWidget* actionWidget, childrenActions.values()) {
        mainLayout->removeWidget(actionWidget);
        delete actionWidget;
    }
    childrenActions.clear();
}


/**
 * @brief DockWidget::updateHeaderText
 * @param text
 */
void DockWidget::updateHeaderText(QString text)
{
    if (descriptionLabel) {
        //descriptionLabel->setText("Select to construct a <br/><i>" + text + "</i>");
        descriptionLabel->setText("Select to construct a <br/>" + text);
    }
}


/**
 * @brief DockWidget::getDockType
 * @return
 */
ToolActionController::DOCK_TYPE DockWidget::getDockType()
{
    return dockType;
}


/**
 * @brief DockWidget::themeChanged
 */
void DockWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QWidget#DOCKWIDGET_MAIN{ background: rgba(0,0,0,0); }"
                  "QScrollArea {"
                  "border: 0px;"
                  "background:" + theme->getAltBackgroundColorHex() + ";"
                  "}");

    if (backButton) {
        backButton->setIcon(theme->getIcon("Actions", "Arrow_Back"));
        //backButton->setStyleSheet(theme->getPushButtonStyleSheet());
    }
}


/**
 * @brief DockWidget::dockActionClicked
 */
void DockWidget::dockActionClicked()
{
    DockActionWidget* senderAction = qobject_cast<DockActionWidget*>(sender());
    emit actionClicked(senderAction);
}


/**
 * @brief DockWidget::setupHeaderLayout
 */
void DockWidget::setupHeaderLayout()
{
    if (!containsHeader) {
        headerLayout = 0;
        descriptionLabel = 0;
        backButton = 0;
        return;
    }

    backButton = new QPushButton(this);
    connect(backButton, SIGNAL(clicked(bool)), this, SIGNAL(backButtonClicked()));

    descriptionLabel = new QLabel("This is a description for dock type " + QString::number(dockType), this);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setAlignment(Qt::AlignCenter);
    descriptionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    descriptionLabel->setStyleSheet("margin: 5px 0px; padding: 0px;");
    descriptionLabel->setFont(QFont(font().family(), 8));

    headerLayout = new QVBoxLayout();
    headerLayout->setMargin(0);
    headerLayout->setSpacing(5);
    headerLayout->addWidget(descriptionLabel);
    headerLayout->addWidget(backButton);
    alignLayout->insertLayout(0, headerLayout);
}

