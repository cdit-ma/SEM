#include "nodetableitem.h"
#include "../Model/node.h"
#include <QVariant>
#include <QDebug>
NodeTableItem::NodeTableItem(Node *node, NodeTableItem *parent)
{
    this->node = node;
    this->parentItem = parent;

    if(parent){
        parent->appendChild(this);
    }
}

NodeTableItem::~NodeTableItem()
{
    if(parentItem){
        parentItem->removeChild(this);
    }
    qDeleteAll(childItems);
}

Node *NodeTableItem::getNode()
{
    return node;
}

void NodeTableItem::appendChild(NodeTableItem *child)
{
    childItems.append(child);
}

void NodeTableItem::removeChild(NodeTableItem *child)
{
    if(childItems.contains(child)){
        childItems.removeOne(child);
    }
}

NodeTableItem *NodeTableItem::child(int row)
{
    return childItems[row];
}

int NodeTableItem::childCount() const
{
    return childItems.count();
}

int NodeTableItem::columnCount() const
{
    if(node->getDataValue("kind") == "AttributeInstance"){
        return 3;
    }
    return 2;
}

QVariant NodeTableItem::data(int column) const
{
    if(column == 0){
        return node->getDataValue("label");
    }else if(column == 1){
        return node->getDataValue("kind");
    }else if(column == 2){
        return node->getDataValue("value");
    }
}

int NodeTableItem::row() const
{
    if(parentItem){
        return parentItem->childItems.indexOf(const_cast<NodeTableItem*>(this));
    }
    return 0;
}

NodeTableItem *NodeTableItem::parent()
{
    return parentItem;
}
