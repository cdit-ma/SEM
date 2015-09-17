#include "toolbarwidgetmenu.h"
#include <QDebug>


/**
 * @brief ToolbarWidgetMenu::ToolbarWidgetMenu
 * @param parent_action
 * @param default_action
 * @param parent
 */
ToolbarWidgetMenu::ToolbarWidgetMenu(ToolbarWidgetAction* parent_action, ToolbarWidgetAction* default_action, QWidget* parent):
    QMenu(parent)
{
    defaultAction = default_action;
    widgetActions = QList<ToolbarWidgetAction*>();

    parentButton = dynamic_cast<QToolButton*>(parent);
    parentMenu = dynamic_cast<ToolbarWidgetMenu*>(parent);

    eventFromMenu = false;
    actionTriggered = false;

    // set parent action and default action
    setParentAction(parent_action);
    setupDefaultAction();

    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(hideMenu(QAction*)));
    connect(this, SIGNAL(aboutToHide()), this, SLOT(close()));

    // this should always have a parent; print warning if it doesn't
    if (!parent) {
        qWarning() << "ToolbarWidgetMenu::ToolbarWidgetMenu - " << this << " doesn't have a parent. This may cause errors.";
    }
}


/**
 * @brief ToolbarWidgetMenu::setParentAction
 * @param widgetAction
 */
void ToolbarWidgetMenu::setParentAction(ToolbarWidgetAction* widgetAction)
{
    parentAction = widgetAction;

    // attach this menu to its parentAction
    if (parentAction) {
        parentAction->setMenu(this);
    }
}


/**
 * @brief ToolbarWidgetMenu::getParentAction
 * this returns this menu's parentAction.
 * @return
 */
ToolbarWidgetAction* ToolbarWidgetMenu::getParentAction()
{
    return parentAction;
}


/**
 * @brief ToolbarWidgetMenu::addWidgetAction
 * This stores the newly added ToolbarWidgetAction to the widgetActions list.
 * @param action
 */
void ToolbarWidgetMenu::addWidgetAction(ToolbarWidgetAction* action)
{
    // if widgetActions contains the defaultAction, remove it
    if (widgetActions.contains(defaultAction)) {
        removeWidgetAction(defaultAction);
    }
    widgetActions.append(action);
    QMenu::addAction(action);
}


/**
 * @brief ToolbarWidgetMenu::removeWidgetAction
 * This removes the provided ToolbarWidgetAction from this menu.
 * @param action
  */
void ToolbarWidgetMenu::removeWidgetAction(ToolbarWidgetAction* action, bool clearing)
{
    if (!action) {
        return;
    }
    widgetActions.removeAll(action);

    // remove action from this menu
    removeAction(action);
}


/**
 * @brief ToolbarWidgetMenu::getWidgetActions
 * This returns the widgetActions list.
 * @return
 */
QList<ToolbarWidgetAction*> ToolbarWidgetMenu::getWidgetActions()
{
    return widgetActions;
}


/**
 * @brief ToolbarWidgetMenu::getWidgetAction
 * This returns the ToolbarWidgetAction contained in this menu that is attached to nodeItem.
 * @param nodeItem
 * @return
 */
ToolbarWidgetAction* ToolbarWidgetMenu::getWidgetAction(NodeItem* nodeItem)
{
    if (nodeItem) {
        foreach (ToolbarWidgetAction* action, widgetActions) {
            NodeItem* actionNode = action->getNodeItem();
            if (actionNode && actionNode == nodeItem) {
                return action;
            }
        }
    }
    return 0;
}


/**
 * @brief ToolbarWidgetMenu::hasWidgetActions
 * @return
 */
bool ToolbarWidgetMenu::hasWidgetActions()
{
    return !widgetActions.isEmpty();
}


/**
 * @brief ToolbarWidgetMenu::clearMenu
 * This clears this menu's actions. If the action is stored in the toolbar
 * it removes it from the menu, otherwise it deletes the action.
 */
void ToolbarWidgetMenu::clearMenu()
{
    while(!widgetActions.isEmpty()){
        ToolbarWidgetAction* action = widgetActions.takeFirst();
        removeWidgetAction(action);
        if(action->isDeletable()){
            delete action;
        }
    }

    // once the menu is empty, display its default action (if it has one)
    setupDefaultAction();
}


/**
 * @brief ToolbarWidgetMenu::enterEvent
 * This picks up if the mouse is hovering over this menu.
 */
void ToolbarWidgetMenu::enterEvent(QEvent *e)
{
    eventFromMenu = true;
    QMenu::enterEvent(e);
}


/**
 * @brief ToolbarWidgetMenu::leaveEvent
 * This picks up if the mouse is leaving this menu.
 */
void ToolbarWidgetMenu::leaveEvent(QEvent * e)
{
    eventFromMenu = false;
    QMenu::leaveEvent(e);
}


/**
 * @brief ToolbarWidgetMenu::mousePressEvent
 * @param event
 */
void ToolbarWidgetMenu::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QMenu::mousePressEvent(event);
    }
}


/**
 * @brief ToolbarWidgetMenu::mouseDoubleClickEvent
 * @param event
 */
void ToolbarWidgetMenu::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QMenu::mouseDoubleClickEvent(event);
    }
}


/**
 * @brief ToolbarWidgetMenu::close
 * This menu is closed by either having one of its actions triggered or the user clicking
 * outside of it. Send a signal to its parent to check if the parent also needs to be hidden.
 */
void ToolbarWidgetMenu::close()
{
    if (parentButton) {
        emit toolbarMenu_hideToolbar(actionTriggered);
    } else if (parentMenu) {
        if (!eventFromMenu && !actionTriggered) {
            parentMenu->closeMenu();
        }
    }

    if (parentAction) {
        parentAction->menuClosed();
    }

    actionTriggered = false;
}


/**
 * @brief ToolbarWidgetMenu::closeMenu
 * This is called by the children menus when they are closed by an event
 * not triggered by them. If the event didn't come from this menu either,
 * send the same check signal to its parent then close it.
 */
void ToolbarWidgetMenu::closeMenu()
{
    if (!eventFromMenu && !actionTriggered) {
        hide();
    }
}


/**
 * @brief ToolbarWidgetMenu::hideMenu
 * If an action in this menu is triggered and that action doesn't have a menu, hide this menu.
 * If a child menu is triggered (when widgetAction is null), hide this menu.
 * @param action
 */
void ToolbarWidgetMenu::hideMenu(QAction* action)
{
    ToolbarWidgetAction* widgetAction = dynamic_cast<ToolbarWidgetAction*>(action);
    if (!widgetAction || (widgetAction && widgetAction->getMenu() == 0)) {
        actionTriggered = true;
        hide();
    } else {
        actionTriggered = false;
    }
}


/**
 * @brief ToolbarWidgetMenu::execMenu
 * This executes this menu next to its parent ToolbarWidgetAction.
 */
void ToolbarWidgetMenu::execMenu()
{
    // can't just check the widgetActions list here
    // some acttions in the menu might not be a ToolbarWidgetAction
    if (QMenu::actions().isEmpty()) {
        return;
    }
    if (parentAction) {
        QMenu::exec(parentAction->getButtonPos());
    }
}


/**
 * @brief ToolbarWidgetMenu::setupDefaultAction
 * If this menu has a default action, add it to the widgetActions list.
 * The default action is displayed when there are no other actions in this menu.
 */
void ToolbarWidgetMenu::setupDefaultAction()
{
    if (defaultAction) {
        widgetActions.append(defaultAction);
        QMenu::addAction(defaultAction);
    }
}

