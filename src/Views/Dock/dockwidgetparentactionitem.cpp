#include "dockwidgetparentactionitem.h"
#include "../../theme.h"

#define ARROW_SIZE 16
#define ITEM_HEIGHT 24

/**
 * @brief DockWidgetParentActionItem::DockWidgetParentActionItem
 * @param text
 * @param parent
 */
DockWidgetParentActionItem::DockWidgetParentActionItem(QAction *action, QWidget *parent) :
    DockWidgetItem(action->text(), parent)
{
    dockAction = action;
    childrenActionGroup = new QActionGroup(this);

    dockActionID = -1;
    if (action) {
        dockActionID = action->property("ID").toInt();
    }

    setCheckable(true);
    setChecked(true);
    setEnabled(true);
    setLayoutDirection(Qt::RightToLeft);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setFixedHeight(ITEM_HEIGHT);

    connect(action, SIGNAL(changed()), this, SLOT(actionChanged()));

    //This can't be done, you are caching your items in a hash a level up.
    //This will keep a pointer to the item, even though it's been deleted...
    //connect(action, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(this, SIGNAL(toggled(bool)), SLOT(setChildrenVisible(bool)));

    actionChanged();
    themeChanged();
}


/**
 * @brief DockWidgetParentActionItem::getAction
 * @return
 */
QAction* DockWidgetParentActionItem::getAction()
{
    return dockAction;
}


/**
 * @brief DockWidgetParentActionItem::addToChildrenActions
 * @param action
 */
void DockWidgetParentActionItem::addToChildrenActions(QAction* action)
{
    childrenActionGroup->addAction(action);
}


/**
 * @brief DockWidgetParentActionItem::setChildrenActions
 * @param actions
 */
void DockWidgetParentActionItem::setChildrenActions(QList<QAction*> actions)
{
    childrenActionGroup = new QActionGroup(this);
    foreach (QAction* action, actions) {
        childrenActionGroup->addAction(action);
    }
}


/**
 * @brief DockWidgetParentActionItem::setToggledState
 * @param toggled
 */
void DockWidgetParentActionItem::setToggledState(bool toggled)
{
    setChecked(toggled);
    setChildrenVisible(toggled);
}


/**
 * @brief DockWidgetParentActionItem::getItemKind
 * @return
 */
DockWidgetItem::DOCKITEM_KIND DockWidgetParentActionItem::getItemKind()
{
    return GROUP_ACTION_ITEM;
}


/**
 * @brief DockWidgetParentActionItem::setProperty
 * @param name
 * @param value
 */
void DockWidgetParentActionItem::setProperty(const char *name, const QVariant &value)
{
    if (dockAction) {
        dockAction->setProperty(name, value);
    }
}


/**
 * @brief DockWidgetParentActionItem::getProperty
 * @param name
 * @return
 */
QVariant DockWidgetParentActionItem::getProperty(const char *name)
{
    return dockAction->property(name);
}


/**
 * @brief DockWidgetParentActionItem::actionChanged
 */
void DockWidgetParentActionItem::actionChanged()
{
    if (dockAction) {
        QString actionText = dockAction->text();
        if (actionText != getText()) {
            setText(actionText);
        }
        setVisible(dockAction->isVisible());
    }
}


/**
 * @brief DockWidgetParentActionItem::themeChanged
 */
void DockWidgetParentActionItem::themeChanged()
{
    Theme* theme = Theme::theme();
    QColor altColor = theme->getAltBackgroundColor().darker(110);

    setIcon(theme->getIcon("Icons", "arrowHeadVerticalToggle"));
    setStyleSheet("QToolButton {"
                  "border-color:" + theme->getDisabledBackgroundColorHex() + ";"
                  "padding: 2px 0px 2px 7px;"
                  "background:" + theme->getAltBackgroundColorHex() + ";}"
                  "QToolButton::checked {"
                  "color:" + theme->getTextColorHex() + ";"
                  "background:" + theme->QColorToHex(altColor) + ";"
                  "}"
                  "QToolButton:hover {"
                  "background:" + theme->getHighlightColorHex() + ";"
                  "color:" + theme->getTextColorHex(theme->CR_SELECTED) + ";"
                  "}");
}


/**
 * @brief DockWidgetParentActionItem::setChildrenVisible
 * @param visible
 */
void DockWidgetParentActionItem::setChildrenVisible(bool visible)
{
    childrenActionGroup->setVisible(visible);
}


/**
 * @brief DockWidgetParentActionItem::enterEvent
 * @param event
 */
void DockWidgetParentActionItem::enterEvent(QEvent *event)
{
    emit hoverEnter(dockActionID);
    QToolButton::enterEvent(event);
}


/**
 * @brief DockWidgetParentActionItem::leaveEvent
 * @param event
 */
void DockWidgetParentActionItem::leaveEvent(QEvent *event)
{
    emit hoverLeave(dockActionID);
    QToolButton::leaveEvent(event);
}
