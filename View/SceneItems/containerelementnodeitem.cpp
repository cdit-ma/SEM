#include "containerelementnodeitem.h"
#include "Assemblies/nodeitemstackcontainer.h"
#include <QDebug>

ContainerElementNodeItem::ContainerElementNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    : NodeItemNew(viewItem, parentItem, CONTAINER_ELEMENT_ITEM)
{
    container = qobject_cast<StackContainerNodeItem*>(parentItem);

    addRequiredData("sortOrder");
    addRequiredData("x");
    addRequiredData("y");
    reloadRequiredData();
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

void ContainerElementNodeItem::setPos(const QPointF &p)
{
    QPointF pos = p;
    if(getContainer()){
        if(!isMoving()){
            //Force it's position if it isn't moving!
            pos = getContainer()->getElementPosition(this);
        }
    }
    NodeItemNew::setPos(pos);
}

void ContainerElementNodeItem::dataChanged(QString keyName, QVariant data)
{
    NodeItemNew::dataChanged(keyName, data);

    if(keyName == "sortOrder" && getContainer() && getContainer()->isSortOrdered()){
        QPoint index = getIndexPosition();
        index.setY(getSortOrder());
        setIndexPosition(index);
        setPos(QPointF());
    }
}

QPointF ContainerElementNodeItem::getNearestGridPoint(QPointF newPos)
{
    if(getContainer()){
        return getContainer()->getElementPosition(this);
    }
    return NodeItemNew::getNearestGridPoint(newPos);
}

