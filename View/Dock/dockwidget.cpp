#include "dockwidget.h"
#include "../theme.h"
#include <QDebug>

/**
 * @brief DockWidget::DockWidget
 * @param parent
 */
DockWidget::DockWidget(ToolActionController* tc, ToolActionController::DOCK_TYPE type, QWidget *parent) : QWidget(parent)
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
    //mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
    //mainLayout->setSpacing(5);

    alignLayout = new QVBoxLayout(this);
    alignLayout->addLayout(mainLayout);
    //alignLayout->addStretch();

    setupHeaderLayout();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}


/**
 * @brief DockWidget::addAction
 * @param action
 */
void DockWidget::addAction(QAction* action)
{
    if (action && !childrenActions.contains(action)) {
        DockActionWidget* dockAction = new DockActionWidget(action, this);
        dockAction->setProperty("kind", action->text());

        if (toolActionController->kindsWithSubActions.contains(action->text())) {
            dockAction->setSubActionRequired(true);
        }

        childrenActions[action] = dockAction;
        mainLayout->addWidget(dockAction, 0, Qt::AlignTop);
        connect(dockAction, SIGNAL(clicked(bool)), this, SLOT(dockActionClicked()));
    }
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
    if (backButton) {
        backButton->setIcon(Theme::theme()->getIcon("Actions", "Arrow_Back"));
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
    //descriptionLabel->setMinimumHeight(100);
    descriptionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //descriptionLabel->setStyleSheet("margin: 0px; padding: 0px;"); // background: red;");

    headerLayout = new QVBoxLayout();
    headerLayout->setMargin(0);
    headerLayout->setSpacing(10);
    headerLayout->addWidget(descriptionLabel, 1);
    headerLayout->addWidget(backButton);

    alignLayout->insertLayout(0, headerLayout);
}

