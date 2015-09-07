#include "toolbarwidgetmenu.h"
#include <QDebug>


/**
 * @brief ToolbarWidgetMenu::ToolbarWidgetMenu
 * @param parent_action
 * @param default_action
 * @param parent
 */
ToolbarWidgetMenu::ToolbarWidgetMenu(ToolbarWidgetAction *parent_action, ToolbarWidgetAction* default_action, QWidget *parent):
    QMenu(parent)
{
    defaultAction = default_action;
    widgetActions = QList<ToolbarWidgetAction*>();

    eventFromMenu = false;
    actionTriggered = false;

    // set parentAction
    setParentAction(parent_action);

    // if the parent of this menu is of type ToolbarWidgetMenu, connect to it
    ToolbarWidgetMenu* parentMenu = qobject_cast<ToolbarWidgetMenu*>(parent);
    if (parentMenu) {
        connect(this, SIGNAL(toolbarMenu_closeParentMenu()), parentMenu, SLOT(closeMenu()));
    }

    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(hideMenu(QAction*)));
    connect(this, SIGNAL(aboutToHide()), this, SLOT(close()));

    // this should always have a parent; print warning if it doesn't
    if (!parent) {
        qWarning() << "ToolbarWidgetMenu::ToolbarWidgetMenu - " << this << " doesn't have a parent. This may cause errors.";
    }
}


/**
 * @brief ToolbarWidgetMenu::execMenu
 * This executes this menu next to its parent ToolbarWidgetAction.
 */
void ToolbarWidgetMenu::execMenu()
{
    if (parentAction) {
        if (widgetActions.count() == 0) {
            if (defaultAction) {
                setupDefaultAction();
            } else if (actions().count() == 0) {
                return;
            }
        }
        exec(parentAction->getButtonPos());
    }
}


/**
 * @brief ToolbarWidgetMenu::addWidgetAction
 * This stores the newly added ToolbarWidgetAction to the widgetActions list.
 * @param action
 */
void ToolbarWidgetMenu::addWidgetAction(ToolbarWidgetAction* action)
{
    // if widgetActions contains the defaultAction, remove it
    if (widgetActions.count() == 1 && defaultAction) {
        widgetActions.removeAll(defaultAction);
        removeAction(defaultAction);
    }
    widgetActions.append(action);
    addAction(action);
}


/**
 * @brief ToolbarWidgetMenu::removeWidgetAction
 * This removes the provided ToolbarWidgetAction from the widgetActions list.
 * @param action
 */
void ToolbarWidgetMenu::removeWidgetAction(ToolbarWidgetAction *action)
{
   if (action && action->isDeletable()) {
       widgetActions.removeAll(action);
       delete action;
   }
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

bool ToolbarWidgetMenu::hasWidgetActions()
{
    return !widgetActions.isEmpty();
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
 * @brief ToolbarWidgetMenu::getWidgetAction
 * This returns the ToolbarWidgetAction contained in this menu that is attached to nodeItem.
 * @param nodeItem
 * @return
 */
ToolbarWidgetAction* ToolbarWidgetMenu::getWidgetAction(NodeItem *nodeItem)
{
    foreach (ToolbarWidgetAction* action, widgetActions) {
        NodeItem* actionNode = action->getNodeItem();
        if (actionNode && actionNode == nodeItem) {
            return action;
        }
    }
    return 0;
}


/**
 * @brief ToolbarWidgetMenu::clearMenu
 * This clears this menu's actions. If the action is stored in the toolbar
 * it removes it from the menu, otherwise it deletes the action.
 */
void ToolbarWidgetMenu::clearMenu()
{
    QMutableListIterator<ToolbarWidgetAction*> it(widgetActions);
    while (it.hasNext()) {
        ToolbarWidgetAction *action = it.next();
        // actions that are stored in the toolbar widget can't be deleted
        if (action && action->isDeletable()) {
            delete action;
        } else {
            removeAction(action);
        }
    }
    widgetActions.clear();
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
 * @brief ToolbarWidgetMenu::close
 * This menu is closed by either having one of its actions triggered or the user clicking
 * outside of it. Send a signal to its parent to check if the parent also needs to be hidden.
 */
void ToolbarWidgetMenu::close()
{
    QToolButton* parentButton = qobject_cast<QToolButton*>(parent());
    if (parentButton) {
        emit toolbarMenu_hideToolbar(actionTriggered);
    } else {
        if (!eventFromMenu && !actionTriggered) {
            emit toolbarMenu_closeParentMenu();
        }
    }

    actionTriggered = false;

    if (parentAction) {
        parentAction->menuClosed();
    }
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
 * If an action in this menu is triggered and that action doesn't
 * have a menu or a child menu is triggered, hide this menu.
 * @param action
 */
void ToolbarWidgetMenu::hideMenu(QAction *action)
{
    ToolbarWidgetAction* widgetAction = dynamic_cast<ToolbarWidgetAction*>(action);
    if (!widgetAction || (widgetAction && widgetAction->getMenu() == 0)) {
        actionTriggered = true;
    } else {
        actionTriggered = false;
    }

    emit toolbarMenu_parentTriggered(actionTriggered);

    if (actionTriggered) {
        hide();
    }
}


/**
 * @brief ToolbarWidgetMenu::setParentAction
 * @param widgetAction
 */
void ToolbarWidgetMenu::setParentAction(ToolbarWidgetAction *widgetAction)
{
    parentAction = widgetAction;

    // attach this menu to its parentAction
    if (widgetAction) {
        parentAction->setMenu(this);
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

