#include "nodeitemstackcontainer.h"
#include "nodeitemcolumnitem.h"
StackContainerNodeItem::StackContainerNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem): ContainerNodeItem(viewItem, parentItem)
{
}

QPointF StackContainerNodeItem::getElementPosition(ContainerElementNodeItem *child)
{
    return gridRect().topLeft() + QPointF(10, child->getIndexPosition().y() * child->getHeight());
}

QPoint StackContainerNodeItem::getElementIndex(ContainerElementNodeItem *child)
{
    QPointF childCenter = child->getCenter() - bodyRect().topLeft();

    return QPoint(0,  childCenter.y() / child->getHeight());
}

