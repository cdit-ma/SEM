#include "nodeviewitem.h"


NodeViewItem::NodeViewItem(NodeAdapter *entity):ViewItem(entity)
{
    this->entity = entity;
}

NodeViewItem::~NodeViewItem()
{

}

int NodeViewItem::getParentID(int depth)
{
    int ID = -1;
    if(entity){
        ID = entity->getParentNodeID(depth);
    }
    return ID;
}

QList<int> NodeViewItem::getTreeIndex()
{
    QList<int> index;
    if(entity){
        index = entity->getTreeIndex();
    }
    return index;
}
