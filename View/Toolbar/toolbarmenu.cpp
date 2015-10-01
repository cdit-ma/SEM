#include "toolbarmenu.h"

#include <QStyle>
#include <QProxyStyle>
#include <QStyleOptionButton>


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
    setStyleSheet("QMenu::item {"
                  "background-color: rgba(0,0,0,0);"
                  "padding: 1px 20px 1px 45px;"
                  "border: none;"
                  "}"
                  "QMenu::item:selected {"
                  "background-color: rgba(230,230,230,250);"
                  "border: 1px solid gray;"
                  "}"
                  "QMenu::icon {"
                  "position: absolute;"
                  "top: 1px;"
                  "right: 3px;"
                  "bottom: 1px;"
                  "left: 4px;"
                  "}"
                  );

    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));
    connect(this, SIGNAL(aboutToShow()), toolbar, SLOT(appendToOpenMenusList()));
    connect(this, SIGNAL(aboutToHide()), toolbar, SLOT(removeFromOpenMenusList()));
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
 * @return
 */
ToolbarMenuAction* ToolbarMenu::getAction(NodeItem* item)
{
    if (item) {
        foreach (ToolbarMenuAction* action, menuActions) {
            NodeItem* actionItem = action->getNodeItem();
            if (actionItem && (actionItem == item)) {
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
    if (event->button() == Qt::LeftButton) {
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
