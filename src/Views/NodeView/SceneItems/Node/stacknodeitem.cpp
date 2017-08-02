#include "stacknodeitem.h"

#include <QDebug>
#include <cmath>

StackNodeItem::StackNodeItem(NodeViewItem *viewItem, NodeItem *parentItem, Qt::Orientation orientation):
    BasicNodeItem(viewItem, parentItem)
{
    setSortOrdered(true);
    setResizeEnabled(false);
    setGridEnabled(false);

    removeRequiredData("width");
    removeRequiredData("height");
    reloadRequiredData();
    this->orientation = orientation;
}

QPointF StackNodeItem::getStemAnchorPoint() const
{
    //QPointF offset;
    return gridRect().topLeft();
}

QPointF StackNodeItem::getElementPosition(BasicNodeItem *child)
{
    int childPos = child->getSortOrder();
    int gridSize = getGridSize();

    QPointF itemOffset = gridRect().topLeft();

    qreal xMod = fmod(itemOffset.x(), gridSize);
    qreal yMod = fmod(itemOffset.y(), gridSize);

    if(xMod > 0){
        itemOffset.rx() += gridSize - xMod;
    }

    if(yMod > 0){
        itemOffset.ry() += gridSize - yMod;
    }

    //Work out the exact position given the things above us.
    foreach(NodeItem* c, getChildNodes()){
        if(c->getSortOrder() < childPos){
            //Add a grid space gap
            if(orientation == Qt::Vertical){
                itemOffset.ry() += c->currentRect().height() + gridSize;
            }else{
                itemOffset.rx() += c->currentRect().width() + (gridSize * 5);
            }
        }
    }

    return itemOffset;// + child->getMarginOffset();
}

void StackNodeItem::childPosChanged()
{
    QList<NodeItem*> children = getChildNodes();
    foreach(NodeItem* child, children){
        if(!child->isMoving()){
            child->setPos(QPointF());
        }
    }
    NodeItem::childPosChanged();
    update();
}
