#ifndef NODEITEMSTACKCONTAINER_H
#define NODEITEMSTACKCONTAINER_H

#include "nodeitemorderedcontainer.h"

class StackContainerNodeItem: public ContainerNodeItem
{
public:
    StackContainerNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem);

    QPointF getElementPosition(ContainerElementNodeItem *child);
    QPoint getElementIndex(ContainerElementNodeItem *child);
};

#endif // NODEITEMSTACKCONTAINER_H
