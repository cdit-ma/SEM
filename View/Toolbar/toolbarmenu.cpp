#include "toolbarmenu.h"

#include <QStyle>
#include <QProxyStyle>
#include <QStyleOptionButton>
#include <QDebug>


/**
 * @brief The MenuStyle class
 * This class is used to style the menu and its items (actions).
 */
class MenuStyle : public QProxyStyle
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
    setStyle(new MenuStyle);

    connect(this, SIGNAL(aboutToShow()), toolbar, SLOT(appendToOpenMenusList()));
    connect(this, SIGNAL(aboutToHide()), toolbar, SLOT(removeFromOpenMenusList()));
    connect(this, SIGNAL(aboutToHide()), toolbar, SLOT(menuActionHovered()));
    connect(this, SIGNAL(hovered(QAction*)), toolbar, SLOT(menuActionHovered(QAction*)));
    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
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
    menuActions.append(action);
    QMenu::addAction(action);
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
 * @brief ToolbarMenu::getAction
 * @param item
 * @param withMenu
 * @return
 */
ToolbarMenuAction* ToolbarMenu::getAction(NodeItem* item, bool withMenu)
{
    if (item) {
        foreach (ToolbarMenuAction* action, menuActions) {
            NodeItem* actionItem = action->getNodeItem();
            if (actionItem && (actionItem == item)) {
                if (withMenu && !action->menu()) {
                    continue;
                }
                return action;
            }
        }
    }
    return 0;
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
 * Ignore all mouse buttons except for the left button.
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
 * @brief ToolbarMenu::setupInfoAction
 */
void ToolbarMenu::setupInfoAction()
{
    if (isEmpty() && infoAction) {
        menuActions.append(infoAction);
        QMenu::addAction(infoAction);
    }
}
