#include "containerelementnodeitem.h"
#include "Assemblies/nodeitemcolumncontainer.h"
#include "Assemblies/nodeitemstackcontainer.h"
#include <QDebug>

ContainerElementNodeItem::ContainerElementNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    : NodeItemNew(viewItem, parentItem, CONTAINER_ELEMENT_ITEM)
{
    container = qobject_cast<StackContainerNodeItem*>(parentItem);

    setMargin(QMarginsF(10,5,10,5));

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

void ContainerElementNodeItem::setPos(const QPointF &pos)
{
    if(getContainer()){
        if(isMoving()){
            setIndexPosition(getContainer()->getElementIndex(this));
        }else{
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
        QPoint index = getIndexPosition();
        index.setY(getSortOrder());
        setIndexPosition(index);
        setPos(QPointF());
    }
}

