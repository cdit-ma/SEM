#include "nodeitemstackcontainer.h"
#include "nodeitemcolumnitem.h"
#include <QDebug>
StackContainerNodeItem::StackContainerNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem):
    ContainerNodeItem(viewItem, parentItem)
{
    setSortOrdered(true);
    setResizeEnabled(false);
    setGridEnabled(false);



    leafPen.setWidthF(.5);
    if(parentItem){
        leafPen.setColor(getBaseBodyColor().darker(110));
    }
    reloadRequiredData();
}

QPointF StackContainerNodeItem::getStemAnchorPoint() const
{
    //QPointF offset;
    return gridRect().topLeft();
}

QPointF StackContainerNodeItem::getElementPosition(ContainerElementNodeItem *child)
{
    int childPos = child->getSortOrder();
    int gridSize = getGridSize();
    QPointF offset = gridRect().topLeft();

    qreal xMod = fmod(offset.x(), getGridSize());
    qreal yMod = fmod(offset.y(), getGridSize());

    if(xMod > 0){
        offset.rx() += gridSize - xMod;
    }
    if(yMod > 0){
        offset.ry() += gridSize - yMod;
    }
    //Work out the exact position given the things above us.
    foreach(NodeItemNew* child, getChildNodes()){
        if(child->getSortOrder() < childPos){
            qreal cY = child->translatedBoundingRect().bottom();
            cY += gridSize;
            if(cY > offset.y()){
                offset.ry ()= cY;
            }
        }
    }
    return offset;
    //offset.ry() = y;
    //return offset;
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
    int stemX = gridRect().x() + 5;

    int maxY = 0;
    int index = 0;
    foreach(NodeItemNew* child, children){
        if(!child->isMoving()){
            child->setPos(QPointF());
        }
        QPointF center = child->getPos();

        QLineF line;
        line.setP2(center);
        line.setP1(QPointF(stemX,line.p2().y()));
        leaves.insert(index++, line);
        if(center.y() > maxY){
            maxY = center.y();
        }
    }
    QLineF stem;
    stem.setP1(QPointF(stemX, gridRect().y()));
    stem.setP2(QPointF(stemX, maxY));
    leaves.insert(index++, stem);
    NodeItemNew::childPosChanged();
    update();
}

void StackContainerNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    RENDER_STATE state = getRenderState(lod);

    ContainerNodeItem::paint(painter, option, widget);
    /*if(isExpanded() && state > RS_BLOCK){
        leafPen.setColor(getBodyColor().darker(150));
        painter->setPen(leafPen);
        painter->drawLines(leaves);
    }*/
}

