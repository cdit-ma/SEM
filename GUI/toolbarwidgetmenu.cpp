#include "toolbarwidgetmenu.h"


/**
 * @brief ToolbarWidgetMenu::ToolbarWidgetMenu
 * @param action
 * @param parent
 */
ToolbarWidgetMenu::ToolbarWidgetMenu(ToolbarWidgetAction *action, QWidget *parent) :
    QMenu(parent)
{
    parentAction = action;
    eventFromMenu = false;
}


/**
 * @brief ToolbarWidgetMenu::exec
 * This executes this menu next to it's ToolbarWidgetAction.
 * @return
 */
QAction *ToolbarWidgetMenu::exec()
{
    return QMenu::exec(parentAction->getButtonPos());
}


/**
 * @brief ToolbarWidgetMenu::addWidgetAction
 * This stores the newly added ToolbarWidgetAction to the widgetActions list.
 * @param action
 */
void ToolbarWidgetMenu::addWidgetAction(ToolbarWidgetAction *action)
{
    widgetActions.append(action);
    connect(action, SIGNAL(menuAttached(QMenu*)), this, SLOT(connectChildMenu(QMenu*)));
}


/**
 * @brief ToolbarWidgetMenu::getWidgetActions
 * This returns the widgetActions list.
 * @return
 */
QList<ToolbarWidgetAction *> ToolbarWidgetMenu::getWidgetActions()
{
    return widgetActions;
}


/**
 * @brief ToolbarWidgetMenu::enterEvent
 * This picks up if the mouse is hovering over this menu.
 */
void ToolbarWidgetMenu::enterEvent(QEvent *)
{
    eventFromMenu = true;
}


/**
 * @brief ToolbarWidgetMenu::leaveEvent
 * This picks up if the mouse is leaving this menu.
 */
void ToolbarWidgetMenu::leaveEvent(QEvent *)
{
    eventFromMenu = false;
}


/**
 * @brief ToolbarWidgetMenu::close
 * This closes this meu along with its children menus.
 * If the event didn't come from this menu, check if it came from its parent menu.
 * @return
 */
bool ToolbarWidgetMenu::close()
{
    for (int i=0; i<widgetActions.count(); i++) {
        if (widgetActions.at(i)->getMenu() != 0) {
            widgetActions.at(i)->getMenu()->close();
        }
    }

    if (!eventFromMenu) {
        emit closeParentMenu();
    }

    return QMenu::close();
}


/**
 * @brief ToolbarWidgetMenu::closeMenu
 * This is called by the children menus when they are closed by an event not from them.
 * If the event didn't come from this menu either then close it.
 */
void ToolbarWidgetMenu::closeMenu()
{
    if (!eventFromMenu) {
        QMenu::close();
    }
}


/**
 * @brief ToolbarWidgetMenu::connectChildMenu
 * @param menu
 */
void ToolbarWidgetMenu::connectChildMenu(QMenu *menu)
{
    ToolbarWidgetMenu* widgetMenu = qobject_cast<ToolbarWidgetMenu*>(menu);
    if (widgetMenu) {
        connect(widgetMenu, SIGNAL(closeParentMenu()), this, SLOT(closeMenu()));
    }
}

