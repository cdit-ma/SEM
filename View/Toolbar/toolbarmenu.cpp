#include "toolbarmenu.h"

#include <QStyle>
#include <QProxyStyle>
#include <QStyleOptionButton>
#include <QDebug>


/**
 * @brief The MenuStyle class
 * This class is used to style the menu and its items (actions).
 */
class MenuStyle2 : public QProxyStyle
{
public:
    int pixelMetric(PixelMetric metric, const QStyleOption* option = 0, const QWidget* widget = 0) const
    {
        int s = QProxyStyle::pixelMetric(metric, option, widget);
        if (metric == QStyle::PM_SmallIconSize) {
            s = 35;
        }
        return s;
    }
};


/**
 * @brief ToolbarMenu::ToolbarMenu
 * @param toolbar
 * @param info_action
 */
ToolbarMenu::ToolbarMenu(ToolbarWidget* toolbar, ToolbarMenuAction* info_action) :
    QMenu(toolbar)
{
    infoAction = info_action;

    setupInfoAction();
    setStyle(new MenuStyle2);

    connect(this, SIGNAL(aboutToShow()), toolbar, SLOT(appendToOpenMenusList()));
    connect(this, SIGNAL(aboutToHide()), toolbar, SLOT(removeFromOpenMenusList()));
    connect(this, SIGNAL(aboutToHide()), toolbar, SLOT(menuActionHovered()));
    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
    connect(this, SIGNAL(hovered(QAction*)), toolbar, SLOT(menuActionHovered(QAction*)));
    connect(this, SIGNAL(toolbarMenu_hoverLeave(QAction*)), toolbar, SLOT(menuActionHovered(QAction*)));
}


/**
 * @brief ToolbarMenu::addAction
 * @param action
 */
void ToolbarMenu::addAction(ToolbarMenuAction* action)
{
    if (!action) {
        return;
    }
    if (infoAction && QMenu::actions().contains(infoAction)) {
        removeAction(infoAction);
    }
    insertMenuAction(action);
}


/**
 * @brief ToolbarMenu::removeAction
 * @param action
 * @param clearing
 */
void ToolbarMenu::removeAction(ToolbarMenuAction* action, bool clearing)
{
    if (!action) {
        return;
    }

    // only remove the action from the menuActions list if this menu isn't clearing
    if (!clearing) {
        menuActions.removeAll(action);
    }

    // remove action from this menu
    QMenu::removeAction(action);

    // if the action is deletable, delete it - infoAction shouldn't be deletable
    if (action->isDeletable()) {
        delete action;
    }
}


/**
 * @brief ToolbarMenu::getActions
 * This returns all menu actions that has matching nodeitems to item.
 * @param item - if this is null, return all menu actions
 * @return
 */
QList<ToolbarMenuAction*> ToolbarMenu::getActions(NodeItem* item)
{
    if (!item) {
        return menuActions;
    }

    QList<ToolbarMenuAction*> itemActions;
    foreach (ToolbarMenuAction* action, menuActions) {
        NodeItem* actionItem = action->getNodeItem();
        if (actionItem && (actionItem == item)) {
            itemActions.append(action);
        }
    }

    return itemActions;
}


/**
 * @brief ToolbarMenu::isEmpty
 * @return
 */
bool ToolbarMenu::isEmpty()
{
    return menuActions.isEmpty();
}


/**
 * @brief ToolbarMenu::clearMenu
 */
void ToolbarMenu::clearMenu()
{
    while (!menuActions.isEmpty()) {
        ToolbarMenuAction* action = menuActions.takeFirst();
        removeAction(action, true);
    }
    setupInfoAction();
}


/**
 * @brief ToolbarMenu::splitActionsWithMenu
 */
void ToolbarMenu::splitActionsWithMenu()
{
    if (getActions().isEmpty()) {
        return;
    }

    QList<ToolbarMenuAction*> actionsWithMenu;
    foreach (ToolbarMenuAction* action, getActions()) {
        if (action->menu()) {
            actionsWithMenu.append(action);
        }
    }

    // add a separator at the end of the menu to separate all actions with & without a menu
    if (!actionsWithMenu.isEmpty()) {
        addSeparator();
    }

    // remove all actions with a menu then re-add them at the bottom
    foreach (ToolbarMenuAction* action, actionsWithMenu) {
        QMenu::removeAction(action);
        QMenu::addAction(action);
    }
}


/**
 * @brief ToolbarMenu::menuTriggered
 * @param action
 */
void ToolbarMenu::menuTriggered(QAction* action)
{
    ToolbarMenuAction* menuAction = qobject_cast<ToolbarMenuAction*>(action);
    emit toolbarMenu_triggered(menuAction);

    // this shouldn't be needed, but just to make sure that this menu hides after it's triggered
    QMenu::close();
}


/**
 * @brief ToolbarMenu::mousePressEvent
 * Ignore all mouse buttons except for the left and right buttons.
 * @param event
 */
void ToolbarMenu::mousePressEvent(QMouseEvent* event)
{
    Qt::MouseButton mb = event->button();
    if (mb == Qt::LeftButton || mb == Qt::RightButton) {
        QMenu::mousePressEvent(event);
    }
}


/**
 * @brief ToolbarMenu::leaveEvent
 * @param event
 */
void ToolbarMenu::leaveEvent(QEvent *event)
{
    emit toolbarMenu_hoverLeave();
    QMenu::leaveEvent(event);
}


/**
 * @brief ToolbarMenu::setupInfoAction
 */
void ToolbarMenu::setupInfoAction()
{
    if (isEmpty() && infoAction) {
        menuActions.append(infoAction);
        QMenu::addAction(infoAction);
    }
}


/**
 * @brief ToolbarMenu::insertMenuAction
 * @param action
 */
void ToolbarMenu::insertMenuAction(ToolbarMenuAction *action)
{
    if (!action) {
        return;
    }

    if (!getActions().isEmpty()) {
        QString actionName = action->text();
        for (int i = 0; i < getActions().count(); i++) {
            ToolbarMenuAction* currentAction = getActions().at(i);
            if (!currentAction) {
                continue;
            }
            QString caName = currentAction->text();
            int compare = actionName.compare(caName, Qt::CaseInsensitive);
            if (compare <= 0) {
                QMenu::insertAction(currentAction, action);
                menuActions.insert(i, action);
                return;
            }
        }
    }

    // if there's currently no action in this menu or the
    // action wasn't inserted, just add it to the menu
    QMenu::addAction(action);
    menuActions.append(action);
}
