#include "nodeitemcolumncontainer.h"
#include "nodeitemcolumnitem.h"
#include <QDebug>
ColumnContainerNodeItem::ColumnContainerNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :ContainerNodeItem(viewItem, parentItem)
{
    columnSpacing = 2;
    columnWidth = 44;
    columnHeight = columnWidth;
    setMinimumWidth(columnWidth * 3);
}


QPointF ColumnContainerNodeItem::getColumnPosition(QPoint index) const
{

    return bodyRect().topLeft() + QPointF((columnWidth + columnSpacing) * (index.x()), (columnHeight + columnSpacing) * index.y());
}

QRectF ColumnContainerNodeItem::getColumnRect(int x)
{
    QRectF rect;
    rect.setWidth(columnWidth + (2 * columnSpacing));
    rect.setHeight(bodyRect().height());
    rect.moveTopLeft(bodyRect().topLeft() + QPointF(x * rect.width(),0));
    return rect;
}

QPointF ColumnContainerNodeItem::getElementPosition(ContainerElementNodeItem *child)
{
    return getColumnPosition(child->getIndexPosition());
}

QPoint ColumnContainerNodeItem::getElementIndex(ContainerElementNodeItem *child)
{
    QPoint currentIndex = child->getIndexPosition();
    if(currentIndex.x() == 0){
        //Initial steup
        if(child->getNodeKind() == Node::NK_INEVENTPORT_INSTANCE){
            currentIndex.setX(1);
        }else if(child->getNodeKind() == Node::NK_OUTEVENTPORT_INSTANCE){
            currentIndex.setX(3);
        }else{
            currentIndex.setX(2);
        }
        currentIndex.setY(child->getSortOrder());
    }else{
        //QPointF childCenter = child->getCenter() - (bodyRect().topLeft() + QPointF(2,2));
        //currentIndex.setX(childCenter.x() / (columnWidth + columnSpacing));
        //currentIndex.setY(childCenter.y() / (columnHeight + columnSpacing));
    }
    return currentIndex;
}

void ColumnContainerNodeItem::childPosChanged()
{
}
