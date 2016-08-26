#include "containerelementnodeitem.h"
#include "Assemblies/nodeitemcolumncontainer.h"
#include <QDebug>

ContainerElementNodeItem::ContainerElementNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    : NodeItemNew(viewItem, parentItem, CONTAINER_ELEMENT_ITEM)
{
    container = qobject_cast<ContainerNodeItem*>(parentItem);

    setMargin(QMarginsF(2,1,2,1));

    addRequiredData("sortOrder");


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

void ContainerElementNodeItem::setPos(const QPointF &pos)
{
    if(getContainer()){
        setIndexPosition(getContainer()->getElementIndex(this));
        if(!isMoving()){
            //Force it's position
            EntityItemNew::setPos(getContainer()->getElementPosition(this));
            return;
        }
    }
    NodeItemNew::setPos(pos);
}

void ContainerElementNodeItem::dataChanged(QString keyName, QVariant data)
{
    NodeItemNew::dataChanged(keyName, data);

    if(keyName == "sortOrder" && getContainer() && getContainer()->isSortOrdered()){
        qCritical() << this << data;
        QPoint index = getIndexPosition();
        index.setY(data.toInt());
        setIndexPosition(index);
        setPos(QPointF());
    }
}

