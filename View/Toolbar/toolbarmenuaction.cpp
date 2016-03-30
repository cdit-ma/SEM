#include "toolbarmenuaction.h"
#include "toolbarmenu.h"
#include <QDebug>
#include "../../theme.h"


/**
 * @brief ToolbarMenuAction::ToolbarMenuAction
 * @param item
 * @param parent
 */
ToolbarMenuAction::ToolbarMenuAction(NodeItem* item, ToolbarMenuAction* parent_action, QWidget* parent) :
    QAction(parent)
{
    //connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    parentAction = parent_action;
    nodeItem = item;
    deletable = true;

    prefixPath = "Items";


    if (nodeItem) {
        actionKind = nodeItem->getNodeKind();
        if (nodeItem->isEntityItem()) {
            setText(nodeItem->getLabel());
        } else {
            setText(actionKind);
        }
    } else {
        qWarning() << "ToolbarMenuAction::ToolbarMenuAction - NodeItem is null.";
    }
    themeChanged();
}


/**
 * @brief ToolbarMenuAction::ToolbarMenuAction
 * @param actionKind
 * @param parent
 */
ToolbarMenuAction::ToolbarMenuAction(QString kind, ToolbarMenuAction* parent_action, QWidget* parent, QString displayedText, QString prefixPath, QString aliasPath) :
    QAction(parent)
{
    Q_UNUSED(aliasPath)
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));

    parentAction = parent_action;
    nodeItem = 0;
    deletable = true;
    actionKind = kind;


    ToolbarWidget* toolbar = dynamic_cast<ToolbarWidget*>(parent);
    QStringList nonDeletableKinds;
    if (toolbar) {
        nonDeletableKinds = toolbar->getNonDeletableMenuActionKinds();
    }
    nonDeletableKinds.append("Info");

    if (nonDeletableKinds.contains(actionKind)) {
        deletable = false;
    }

    if (actionKind == "Info") {
        //setEnabled(false);
    }

    if (!displayedText.isEmpty()) {
        setText(displayedText);
    } else {
        setText(actionKind);
    }

    if (prefixPath.isEmpty()) {
        prefixPath = "Items";
    }

    this->prefixPath = prefixPath;

    themeChanged();
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

void ToolbarMenuAction::themeChanged()
{
    updateIcon();
}

void ToolbarMenuAction::updateIcon()
{
    QString prefix = prefixPath;
    QString name = actionKind;

    if (nodeItem && nodeItem->isEntityItem()){
        EntityItem* entityItem = (EntityItem*) nodeItem;
        prefix = entityItem->getIconPrefix();
        name = entityItem->getIconURL();
    }

    QIcon icon = Theme::theme()->getIcon(prefix, name);

    if(icon.isNull()){
        icon = Theme::theme()->getIcon("Actions", "Help");
    }

    setIcon(icon);
}

