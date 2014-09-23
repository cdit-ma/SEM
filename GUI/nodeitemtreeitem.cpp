#include "nodeitemtreeitem.h"
#include <QDebug>

NodeItemTreeItem::NodeItemTreeItem()
{
    this->nodeItem = 0;
    this->parentItem = 0;
}

NodeItemTreeItem::NodeItemTreeItem(NodeItem *nodeItem, NodeItemTreeItem *parent)
{
    this->nodeItem = nodeItem;
    setParent(parent);
}

NodeItemTreeItem::~NodeItemTreeItem()
{
    qDeleteAll(childItems);
}

void NodeItemTreeItem::appendChild(NodeItemTreeItem *child)
{
    qCritical() << "Adding Child";
    childItems.append(child);
    child->setParent(this);
}

void NodeItemTreeItem::setParent(NodeItemTreeItem *parent)
{
    parentItem = parent;
}

NodeItemTreeItem *NodeItemTreeItem::child(int row)
{
    return childItems.at(row);
}

int NodeItemTreeItem::childCount() const
{
    qCritical() << "CHILD COUNT" << childItems.count();
    return childItems.count();
}

int NodeItemTreeItem::columnCount() const
{
    //Type, Label, ID
    return 3;
}

QVariant NodeItemTreeItem::data(int column) const
{

    switch(column){
    case 0:{
        if(nodeItem){
            return nodeItem->node->getDataValue("label");
        }else{
            return "Name";
        }
    }
    case 1:{
        if(nodeItem){
            return nodeItem->node->getDataValue("kind");
        }else{
            return "Kind";
        }
    }
    case 2:{
        if(nodeItem){
            return nodeItem->node->getID();
        }else{
            return "ID";
        }
    }
    default:
        break;
    }
    return QVariant();
}

int NodeItemTreeItem::row() const
{
    if (parentItem){
        qCritical() << "GG";
        return parentItem->childItems.indexOf(const_cast<NodeItemTreeItem*>(this));
    }
    return 0;
}

NodeItemTreeItem *NodeItemTreeItem::parent()
{
    return parentItem;

}
