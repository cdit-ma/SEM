#include "nodeviewitem.h"


NodeViewItem::NodeViewItem(NodeAdapter *entity):ViewItem(entity)
{
    this->entity = entity;

    connect(entity, SIGNAL(dataChanged(QString,QVariant)), this, SLOT(dataChanged(QString,QVariant)));
}


NodeViewItem::~NodeViewItem()
{
}

VIEW_ASPECT NodeViewItem::getViewAspect()
{
    if(entity){
        return entity->getViewAspect();
    }
    return VA_NONE;
}

int NodeViewItem::getParentID(int depth)
{
    int ID = -1;
    if(entity){
        ID = entity->getParentNodeID(depth);
    }
    return ID;
}

NODE_CLASS NodeViewItem::getNodeClass()
{

    NODE_CLASS nodeClass = NC_NONE;
    if(entity){
        nodeClass = entity->getNodeClass();
    }
    return nodeClass;
}

bool NodeViewItem::isInModel()
{
    bool isInModel = false;
    if(entity){
        isInModel = entity->isInModel();
    }
    return isInModel;
}

QList<int> NodeViewItem::getTreeIndex()
{
    QList<int> index;
    if(entity){
        index = entity->getTreeIndex();
    }
    return index;
}



bool NodeViewItem::isAncestorOf(NodeViewItem *item)
{
    QList<int> thisTree = getTreeIndex();
    QList<int> thatTree = item->getTreeIndex();

    if(this == item){
        return true;
    }

    if(thisTree.size() >= thatTree.size()){
        return false;
    }

    for(int i=0; i< thisTree.size(); i++){
        if(thisTree[i] != thatTree[i]){
            return false;
        }
    }
    return true;
}

void NodeViewItem::dataChanged(QString key_name, QVariant data)
{
    if(key_name == "label"){
        emit labelChanged(data.toString());
    }
}
