#include "toolbarmenuaction.h"
#include "toolbarmenu.h"
#include <QDebug>


/**
 * @brief ToolbarMenuAction::ToolbarMenuAction
 * @param item
 * @param parent
 */
ToolbarMenuAction::ToolbarMenuAction(NodeItem* item, QWidget* parent) :
    QAction(parent)
{
    nodeItem = item;
    deletable = true;

    if (nodeItem) {
        actionKind = nodeItem->getNodeKind();
        setIcon(QIcon(QPixmap::fromImage(QImage(":/Items/" + actionKind))));
        if (actionKind.endsWith("Definitions")) {
            setText(actionKind);
        } else {
            setText(nodeItem->getNodeLabel());
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
ToolbarMenuAction::ToolbarMenuAction(QString kind, QWidget* parent, QString displayedText, QString iconPath) :
    QAction(parent)
{
    nodeItem = 0;
    deletable = true;
    actionKind = kind;

    if (actionKind == "ComponentInstance" || actionKind == "ComponentImpl" || actionKind == "BlackBoxInstance" ||
            actionKind == "InEventPortDelegate" || actionKind == "OutEventPortDelegate") {
        deletable = false;
    }

    if (actionKind == "Info") {
        deletable = false;
        setEnabled(false);
    }

    if (!displayedText.isEmpty()) {
        setText(displayedText);
    } else {
        setText(actionKind);
    }

    setIcon(QIcon(QPixmap::fromImage(QImage(iconPath + actionKind))));
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
QString ToolbarMenuAction::getNodeItemID()
{
   if (nodeItem) {
       return nodeItem->getID();
   }
   return "";
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

