#include "toolbarwidgetmenu.h"

#include <QDebug>


/**
 * @brief ToolbarWidgetMenu::ToolbarWidgetMenu
 * @param action
 * @param parent
 */
ToolbarWidgetMenu::ToolbarWidgetMenu(ToolbarWidgetAction *widgetAction, ToolbarWidgetAction* action, QWidget *parent) :
    QMenu(parent)
{
    defaultAction = action;
    widgetActions = QList<ToolbarWidgetAction*>();

    eventFromMenu = false;
    actionTriggered = false;

    // set parentAction
    if (widgetAction) {
        setParentAction(widgetAction);
    }

    // if the parent of this menu is of type ToolbarWidgetMenu, connect to it
    ToolbarWidgetMenu* parentMenu = qobject_cast<ToolbarWidgetMenu*>(parent);
    if (parentMenu) {
        connect(this, SIGNAL(toolbarMenu_connectToParentMenu(ToolbarWidgetMenu*)), parentMenu, SLOT(connectChildMenu(ToolbarWidgetMenu*)));
        emit toolbarMenu_connectToParentMenu(this);
    }

    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(hideMenu(QAction*)));
    connect(this, SIGNAL(aboutToHide()), this, SLOT(close()));

    // this should always have a parent; print warning if it doesn't
    if (!parent) {
        qDebug() << "WARNING: " << this << " doesn't have a parent. This may cause errors.";
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
 * @brief ToolbarWidgetMenu::getWidgetActions
 * This returns the widgetActions list.
 * @return
 */
QList<ToolbarWidgetAction*> ToolbarWidgetMenu::getWidgetActions()
{
    return widgetActions;
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
        if (action->isDeletable()) {
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
 * If the close event didn't come from this menu, check if it came from its parent menu.
 */
void ToolbarWidgetMenu::close()
{
    //emit resetActionState();
    if (!eventFromMenu || actionTriggered) {
        QToolButton* button = qobject_cast<QToolButton*>(parent());
        if (button) {
            emit toolbarMenu_hideToolbar(actionTriggered);
        } else {
            emit toolbarMenu_closeParentMenu(actionTriggered);
        }
    }
    actionTriggered = false;
}


/**
 * @brief ToolbarWidgetMenu::closeMenu
 * This is called by the children menus when they are closed by an
 * event not from them. If the event didn't come from this menu either,
 * send the same check signal to its parent then close it.
 */
void ToolbarWidgetMenu::closeMenu(bool triggered)
{
    if (!eventFromMenu || triggered) {
        QToolButton* button = qobject_cast<QToolButton*>(parent());
        if (button) {
            emit toolbarMenu_hideToolbar(triggered);
        } else {
            emit toolbarMenu_closeParentMenu(triggered);
        }
        //emit resetActionState();
        hide();
    }
}


/**
 * @brief ToolbarWidgetMenu::hideMenu
 * If an action in this menu is triggered and that action doesn't have a menu, hide this menu.
 * @param action
 */
void ToolbarWidgetMenu::hideMenu(QAction *action)
{
    ToolbarWidgetAction* widgetAction = dynamic_cast<ToolbarWidgetAction*>(action);
    if (!widgetAction || (widgetAction && widgetAction->getMenu() == 0)) {
        actionTriggered = true;
        hide();
        close();
    }
    actionTriggered = false;
}


/**
 * @brief ToolbarWidgetMenu::connectChildMenu
 * This connects the child menu to this menu and its parentAction.
 * @param menu
 */
void ToolbarWidgetMenu::connectChildMenu(ToolbarWidgetMenu* menu)
{
    connect(menu, SIGNAL(toolbarMenu_closeParentMenu(bool)), this, SLOT(closeMenu(bool)));
}


/**
 * @brief ToolbarWidgetMenu::setParentAction
 * @param widgetAction
 */
void ToolbarWidgetMenu::setParentAction(ToolbarWidgetAction *widgetAction)
{
    // attach this menu to its parentAction
    parentAction = widgetAction;
    parentAction->setMenu(this);
    //connect(this, SIGNAL(resetActionState()), parentAction, SLOT(actionButtonUnclicked()));
}


/**
 * @brief ToolbarWidgetMenu::setupDefaultAction
 */
void ToolbarWidgetMenu::setupDefaultAction()
{
    if (defaultAction) {
        widgetActions.append(defaultAction);
        QMenu::addAction(defaultAction);
    }
}

