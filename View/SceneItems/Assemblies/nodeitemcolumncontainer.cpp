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

    return bodyRect().topLeft() + QPointF((columnWidth + columnSpacing) * index.x(), (columnHeight + columnSpacing) * index.y());
}

QRectF ColumnContainerNodeItem::getColumnRect(int x)
{
    QRectF rect;
    rect.setWidth(columnWidth + (2 * columnSpacing));
    rect.setHeight(bodyRect().height());
    rect.moveTopLeft(bodyRect().topLeft() + QPointF(x * rect.width(),0));
    return rect;
}

void ColumnContainerNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    ContainerNodeItem::paint(painter, option, widget);
    if(state > RS_BLOCK){
        painter->setClipRect(option->exposedRect);

        if(isGridVisible()){
            painter->setBrush(Qt::red);
            painter->drawRect(getColumnRect(0));
            painter->setBrush(Qt::green);
            painter->drawRect(getColumnRect(2));
        }
    }
}

QPointF ColumnContainerNodeItem::getElementPosition(ContainerElementNodeItem *child)
{
    return getColumnPosition(child->getIndexPosition());
}

QPoint ColumnContainerNodeItem::getElementIndex(ContainerElementNodeItem *child)
{
    QPointF childCenter = child->getCenter() - (bodyRect().topLeft() + QPointF(2,2));

    QPoint index;
    index.setX(childCenter.x() / (columnWidth + columnSpacing));
    index.setY(childCenter.y() / (columnHeight + columnSpacing));
    return index;
}
