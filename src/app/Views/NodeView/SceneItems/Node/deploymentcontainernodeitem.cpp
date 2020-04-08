#include "deploymentcontainernodeitem.h"

DeploymentContainerNodeItem::DeploymentContainerNodeItem(NodeViewItem *viewItem, NodeItem *parentItem)
	: BasicNodeItem(viewItem, parentItem)
{
    addRequiredData(KeyName::IsLateJoiner);
    reloadRequiredData();

    setIcon(EntityRect::MAIN_ICON_OVERLAY, {"Icons", "clock"});
}

void DeploymentContainerNodeItem::dataChanged(const QString& key_name, const QVariant& data)
{
    if (isDataRequired(key_name)) {
        if (key_name == KeyName::IsLateJoiner) {
            bool is_key = data.toBool();
            setIconVisible(EntityRect::MAIN_ICON_OVERLAY, is_key);
        }
        BasicNodeItem::dataChanged(key_name, data);
    }
}