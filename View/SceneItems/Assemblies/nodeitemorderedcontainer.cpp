#include "nodeitemorderedcontainer.h"

NodeItemOrderedContainer::NodeItemOrderedContainer(NodeViewItem *viewItem, NodeItemNew *parentItem):NodeItemContainer(viewItem, parentItem)
{
    container = qobject_cast<NodeItemOrderedContainer*>(parentItem);




    //This changes our position!
    setMargin(QMarginsF(1,1,1,1));
    if(container){
        //Force update
        setPos(QPointF(0,0));
    }
}

NodeItemOrderedContainer *NodeItemOrderedContainer::getContainerParent() const
{
    return container;
}

QPoint NodeItemOrderedContainer::getIndexPosition() const
{
    return indexPosition;
}

void NodeItemOrderedContainer::setIndexPosition(QPoint point)
{
    indexPosition = point;
    //Force an update.
    setPos(QPointF());
}

void NodeItemOrderedContainer::setPos(const QPointF &pos)
{
    if(getContainerParent()){
        if(isMoving()){
            indexPosition = getContainerParent()->getIndexPositionForChild(this);
        }else{
            //Force it's position
            EntityItemNew::setPos(getContainerParent()->getFixedPositionForChild(this));
            return;
        }
    }
    NodeItemContainer::setPos(pos);
}

