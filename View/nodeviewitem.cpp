#include "nodeviewitem.h"


NodeViewItem::NodeViewItem(NodeAdapter *entity):ViewItem(entity)
{
    this->entity = entity;
}

int NodeViewItem::getParentID(int depth)
{
    int ID = -1;
    if(entity){
        ID = entity->getParentID(depth);
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
