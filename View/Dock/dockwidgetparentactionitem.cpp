#include "dockwidgetparentactionitem.h"

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
    qCritical() << "SETTING: ID: " << dockActionID;

    setEnabled(true);
    setCheckable(true);
    setChecked(true);
    setLayoutDirection(Qt::RightToLeft);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setIconSize(ARROW_SIZE, ARROW_SIZE);
    setFixedHeight(ITEM_HEIGHT);

    connect(action, SIGNAL(changed()), this, SLOT(actionChanged()));
    connect(action, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
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
    QColor altColor = theme->getAltBackgroundColor().darker(120);
    //QColor altColor = theme->getBackgroundColor().lighter(130);

    setIcon(theme->getIcon("Actions", "Arrow_Down"));
    setStyleSheet("QToolButton {"
                  "border-color:" + theme->getDisabledBackgroundColorHex() + ";"
                  "padding: 2px 0px 2px 7px;"
                  "background:" + theme->getAltBackgroundColorHex() + ";}"
                  "QToolButton::checked {"
                  "background:" + theme->QColorToHex(altColor) + ";"
                  "}"
                  "QToolButton:hover{ background:" + theme->getHighlightColorHex() + ";}");

    /*setStyleSheet("QToolButton {"
                  "border-color:" + theme->getBackgroundColorHex() + ";"
                  "padding: 2px 0px 2px 7px;"
                  "background:" + theme->getBackgroundColorHex() + ";}"
                  "QToolButton::checked {"
                  "background:" + theme->QColorToHex(altColor) + ";"
                  "}"
                  "QToolButton:hover{ background:" + theme->getHighlightColorHex() + ";}");
                  */
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


