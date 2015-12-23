#include "toolbarmenuaction.h"
#include "toolbarmenu.h"
#include <QDebug>


/**
 * @brief ToolbarMenuAction::ToolbarMenuAction
 * @param item
 * @param parent
 */
ToolbarMenuAction::ToolbarMenuAction(NodeItem* item, ToolbarMenuAction* parent_action, QWidget* parent) :
    QAction(parent)
{
    parentAction = parent_action;
    nodeItem = item;
    deletable = true;

    if (nodeItem) {
        actionKind = nodeItem->getNodeKind();
        if (nodeItem->isEntityItem()) {
            EntityItem* entityItem = (EntityItem*)nodeItem;
            setIcon(QIcon(QPixmap::fromImage(QImage(":/" + entityItem->getIconPrefix() + "/" + entityItem->getIconURL()))));
            setText(nodeItem->getDataValue("label").toString());
        } else {
            setIcon(QIcon(QPixmap::fromImage(QImage(":/Items/" + actionKind))));
            setText(actionKind);
        }
    } else {
        qWarning() << "ToolbarMenuAction::ToolbarMenuAction - NodeItem is null.";
    }
}


/**
 * @brief ToolbarMenuAction::ToolbarMenuAction
 * @param actionKind
 * @param parent
 */
ToolbarMenuAction::ToolbarMenuAction(QString kind, ToolbarMenuAction* parent_action, QWidget* parent, QString displayedText, QString iconPath) :
    QAction(parent)
{
    parentAction = parent_action;
    nodeItem = 0;
    deletable = true;
    actionKind = kind;

    // this is the list of actios from the addMenu in the toolbar that has a menu
    QStringList nonDeletableKinds;
    nonDeletableKinds.append("ComponentInstance");
    nonDeletableKinds.append("ComponentImpl");
    nonDeletableKinds.append("BlackBoxInstance");
    nonDeletableKinds.append("InEventPortDelegate");
    nonDeletableKinds.append("OutEventPortDelegate");
    nonDeletableKinds.append("OutEventPortImpl");
    nonDeletableKinds.append("AggregateInstance");
    nonDeletableKinds.append("VectorInstance");
    nonDeletableKinds.append("Process");
    nonDeletableKinds.append("Info");

    if (nonDeletableKinds.contains(actionKind)) {
        deletable = false;
    }

    if (actionKind == "Info") {
        setEnabled(false);
    }

    if (!displayedText.isEmpty()) {
        setText(displayedText);
    } else {
        setText(actionKind);
    }

    if (iconPath.isEmpty()) {
        iconPath = ":/Items/" + actionKind;
    }

    QPixmap pixmap = QPixmap::fromImage(QImage(iconPath));
    if (pixmap.isNull()) {
        qWarning() << "ToolbarMenuAction::ToolbarMenuAction - Pixmap is null for " + actionKind + ". Check icon path.";
        return;
    }

    /*
    // resize icons for functions and their classes
    if (iconPath.contains("Function")) {
        pixmap = pixmap.scaled(QSize(30,30), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    */

    setIcon(QIcon(pixmap));
}


/**
 * @brief ToolbarMenuAction::getParentAction
 * @return
 */
ToolbarMenuAction *ToolbarMenuAction::getParentAction()
{
    return parentAction;
}


/**
 * @brief ToolbarMenuAction::getParentActionKind
 * @return
 */
QString ToolbarMenuAction::getParentActionKind()
{
    if (parentAction) {
        return parentAction->getActionKind();
    }
    return "";
}


/**
 * @brief ToolbarMenuAction::getNodeItem
 * @return
 */
NodeItem* ToolbarMenuAction::getNodeItem()
{
    return nodeItem;
}


/**
 * @brief ToolbarMenuAction::getNodeItemID
 * @return
 */
int ToolbarMenuAction::getNodeItemID()
{
    if (nodeItem) {
        return nodeItem->getID();
    }
    return -1;
}


/**
 * @brief ToolbarMenuAction::getActionKind
 * @return
 */
QString ToolbarMenuAction::getActionKind()
{
    return actionKind;
}


/**
 * @brief ToolbarMenuAction::isDeletable
 * @return
 */
bool ToolbarMenuAction::isDeletable()
{
    return deletable;
}

