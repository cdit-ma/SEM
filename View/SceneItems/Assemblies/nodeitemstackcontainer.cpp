#include "nodeitemstackcontainer.h"
#include "nodeitemcolumnitem.h"
NodeItemStackContainer::NodeItemStackContainer(NodeViewItem *viewItem, NodeItemNew *parentItem): NodeItemOrderedContainer(viewItem, parentItem)
{
}

QPointF NodeItemStackContainer::getFixedPositionForChild(NodeItemOrderedContainer *child)
{
    return gridRect().topLeft() + QPointF(10, child->getIndexPosition().y() * child->getHeight());
}

QPoint NodeItemStackContainer::getIndexPositionForChild(NodeItemOrderedContainer *child)
{
    return QPoint(0,  (child->getPos().y() - gridRect().top()) / child->getHeight());
}

