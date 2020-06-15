#ifndef DEPLOYMENTCONTAINER_NODEITEM_H
#define DEPLOYMENTCONTAINER_NODEITEM_H

#include "basicnodeitem.h"

class DeploymentContainerNodeItem: public BasicNodeItem
{
    Q_OBJECT
    
public:
    DeploymentContainerNodeItem(NodeViewItem *viewItem, NodeItem *parentItem);
    
protected:
    void dataChanged(const QString& key_name, const QVariant& data) override;
};

#endif // DEPLOYMENTCONTAINER_NODEITEM_H
