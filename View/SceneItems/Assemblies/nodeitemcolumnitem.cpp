#include "nodeitemcolumnitem.h"
#include <QDebug>
NodeItemColumnItem::NodeItemColumnItem(NodeViewItem *viewItem, NodeItemNew *parentItem):NodeItemContainer(viewItem, parentItem)
{
    container = qobject_cast<NodeItemColumnContainer*>(parentItem);
    setIgnorePosition(true);

    column = 0;
    row = 0;

    if(viewItem){
        if(viewItem->getNodeKind() == Node::NK_OUTEVENTPORT_INSTANCE || viewItem->getNodeKind() == Node::NK_OUTEVENTPORT_DELEGATE){
            column = 2;
        }else if(viewItem->getNodeKind() == Node::NK_INEVENTPORT_INSTANCE || viewItem->getNodeKind() == Node::NK_INEVENTPORT_DELEGATE){
            column = 0;
        }else if(viewItem->getNodeKind() == Node::NK_ATTRIBUTE_INSTANCE){
            column = 1;
        }
    }


    //This changes our position!
    setMargin(QMarginsF(1,1,1,1));

    //Force update
    setPos(QPointF(0,0));


}

NodeItemColumnContainer *NodeItemColumnItem::getColumnParent()
{
    return container;
}

int NodeItemColumnItem::getColumn()
{
    return column;
}

int NodeItemColumnItem::getRow()
{
    return row;
}

void NodeItemColumnItem::setPos(const QPointF &pos)
{
    if(isMoving()){
        if(getColumnParent()){
            QPoint point = getColumnParent()->getIndexForChild(this);
            this->column = point.x();
            this->row = point.y();
        }
        NodeItemNew::setPos(pos);
    }else{
        QPointF newPos = pos;
        if(getColumnParent()){
            newPos = getColumnParent()->getPositionForChild(this);
        }
        EntityItemNew::setPos(newPos);
    }
}

