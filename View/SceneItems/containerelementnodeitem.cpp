#include "containerelementnodeitem.h"
#include "Assemblies/nodeitemcolumncontainer.h"

ContainerElementNodeItem::ContainerElementNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    : NodeItemNew(viewItem, parentItem, CONTAINER_ELEMENT_ITEM)
{
    container = qobject_cast<ContainerNodeItem*>(parentItem);


    if(container){
        //setIndexPosition(QPoint());
        //setPos(QPointF());
    }
}

ContainerNodeItem *ContainerElementNodeItem::getContainer() const
{
    return container;
}

QPoint ContainerElementNodeItem::getIndexPosition() const
{
    return indexPosition;
}

void ContainerElementNodeItem::setIndexPosition(QPoint point)
{
    indexPosition = point;
}

void ContainerElementNodeItem::setPos(const QPointF &pos)
{
    if(getContainer()){
        if(isMoving()){
            setIndexPosition(getContainer()->getElementIndex(this));
        }else{
            //Force it's position
            EntityItemNew::setPos(getContainer()->getElementPosition(this));
            return;
        }
    }
    NodeItemNew::setPos(pos);
}

