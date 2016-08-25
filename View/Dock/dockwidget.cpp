#include "dockwidget.h"
#include <QDebug>

/**
 * @brief DockWidget::DockWidget
 * @param parent
 */
DockWidget::DockWidget(ToolActionController* tc, ToolActionController::DOCK_TYPE type, QWidget *parent) : QWidget(parent)
{
    toolActionController = tc;
    dockType = type;
    mainLayout = new QVBoxLayout();
    alignLayout = new QVBoxLayout(this);
    alignLayout->addLayout(mainLayout);
    alignLayout->addStretch();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


/**
 * @brief DockWidget::addAction
 * @param action
 */
void DockWidget::addAction(QAction* action)
{
    if (action && !childrenActions.contains(action)) {
        QString kind = action->text();
        action->setProperty("kind", kind);

        DockActionWidget* dockAction = new DockActionWidget(action, this);
        if (toolActionController->kindsWithSubActions.contains(action->text())) {
            dockAction->requiresSubAction(true);
        }

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
 * @brief DockWidget::getDockType
 * @return
 */
ToolActionController::DOCK_TYPE DockWidget::getDockType()
{
    return dockType;
}


/**
 * @brief DockWidget::dockActionClicked
 */
void DockWidget::dockActionClicked()
{
    DockActionWidget* senderAction = qobject_cast<DockActionWidget*>(sender());
    emit dockActionClicked(senderAction->getAction(), dockType);
}

