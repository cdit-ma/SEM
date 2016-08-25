#include "nodeitemcolumncontainer.h"
#include "nodeitemcolumnitem.h"
#include <QDebug>
NodeItemColumnContainer::NodeItemColumnContainer(NodeViewItem *viewItem, NodeItemNew *parentItem):NodeItemOrderedContainer(viewItem, parentItem)
{
    columnSpacing = 10;
    columnWidth = DEFAULT_SIZE * 2.5;
    columnHeight = DEFAULT_SIZE / 2;
    setBodyPadding(QMarginsF(0,0,0,0));
    setMargin(QMarginsF(0,0,0,0));
}


QPointF NodeItemColumnContainer::getColumnPosition(QPoint index) const
{
    return bodyRect().topLeft() + QPointF((columnWidth + columnSpacing) * index.x(), (columnHeight + columnSpacing) * index.y());
}

QRectF NodeItemColumnContainer::getColumnRect(int x)
{
    QRectF rect;
    rect.setWidth(columnWidth + columnSpacing);
    rect.setHeight(bodyRect().height());
    rect.moveTopLeft(bodyRect().topLeft() + QPointF(x * rect.width(),0));
    return rect;
}

void NodeItemColumnContainer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    NodeItemContainer::paint(painter, option, widget);
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

QPointF NodeItemColumnContainer::getFixedPositionForChild(NodeItemOrderedContainer *child)
{
    return getColumnPosition(child->getIndexPosition());
}

QPoint NodeItemColumnContainer::getIndexPositionForChild(NodeItemOrderedContainer *child)
{
    QPointF childCenter = child->getCenter() - bodyRect().topLeft();

    QPoint index;
    index.setX(childCenter.x() / (columnWidth + columnSpacing));
    index.setY(childCenter.y() / (columnHeight + columnSpacing));
    return index;
}
