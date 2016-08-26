#include "nodeitemstackcontainer.h"
#include "nodeitemcolumnitem.h"
#include <QDebug>
StackContainerNodeItem::StackContainerNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem):
    ContainerNodeItem(viewItem, parentItem)
{
    setSortOrdered(true);

    leafPen.setWidth(1);
    if(parentItem){
        leafPen.setColor(getBodyColor().darker(110));
    }
    reloadRequiredData();
}

QPointF StackContainerNodeItem::getStemAnchorPoint() const
{
    QPointF offset;
    offset.setY(gridRect().y());
    offset.setX(getElementRect(ER_MAIN_ICON).center().x());
    return offset;
}

QPointF StackContainerNodeItem::getElementPosition(ContainerElementNodeItem *child)
{
    int childPos = child->getSortOrder();

    QPointF offset = getStemAnchorPoint();

    int gap = 0;
    //Work out the exact position given the things above us.
    foreach(NodeItemNew* child, getChildNodes()){
        if(child->getSortOrder() < childPos){
            offset.ry() += child->boundingRect().height() + gap;
        }
    }
    return offset;
}

QPoint StackContainerNodeItem::getElementIndex(ContainerElementNodeItem *child)
{
    //Don't change.
    return child->getIndexPosition();
}

void StackContainerNodeItem::childPosChanged()
{
    QList<NodeItemNew*> children = getChildNodes();
    leaves.clear();
    leaves = QVector<QLineF>(children.size() + 1);
    int stemX = gridRect().x() + 10;

    int maxY = 0;
    foreach(NodeItemNew* child, children){
        if(!child->isMoving()){
            child->setPos(QPointF());
        }
        QPointF center = child->getCenter();

        QLineF line;
        line.setP2(center);
        line.setP1(QPointF(stemX,line.p2().y()));
        leaves  << line;
        if(center.y() > maxY){
            maxY = center.y();
        }
    }
    QLineF stem;
    stem.setP1(QPointF(stemX, gridRect().y()));
    stem.setP2(QPointF(stemX, maxY));
    leaves.append(stem);
    NodeItemNew::childPosChanged();
    update();
}

void StackContainerNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    RENDER_STATE state = getRenderState(lod);

    ContainerNodeItem::paint(painter, option, widget);
    if(isExpanded() && state > RS_BLOCK){
        leafPen.setColor(getBodyColor().darker(150));
        painter->setPen(leafPen);
        painter->drawLines(leaves);
    }
}

