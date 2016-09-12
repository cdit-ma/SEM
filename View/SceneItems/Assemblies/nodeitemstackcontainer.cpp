#include "nodeitemstackcontainer.h"
#include "nodeitemcolumnitem.h"
#include <QDebug>
#include <cmath>
StackContainerNodeItem::StackContainerNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem):
    ContainerNodeItem(viewItem, parentItem)
{
    setSortOrdered(true);
    setResizeEnabled(false);
    setGridEnabled(false);

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

    QPointF itemOffset = gridRect().topLeft();

    qCritical() << itemOffset;
    qreal xMod = fmod(itemOffset.x(), gridSize);
    qreal yMod = fmod(itemOffset.y(), gridSize);

    if(xMod > 0){
        itemOffset.rx() += gridSize - xMod;
    }
    if(yMod > 0){
        itemOffset.ry() += gridSize - yMod;
    }

    //Work out the exact position given the things above us.
    foreach(NodeItemNew* c, getChildNodes()){
        if(c->getSortOrder() < childPos){
            //Add a grid space gap
            itemOffset.ry() += c->currentRect().height() + gridSize;
        }
    }

    return itemOffset;// + child->getMarginOffset();
}

void StackContainerNodeItem::childPosChanged()
{
    QList<NodeItemNew*> children = getChildNodes();
    foreach(NodeItemNew* child, children){
        if(!child->isMoving()){
            child->setPos(QPointF());
        }
    }
    NodeItemNew::childPosChanged();
    update();
}
