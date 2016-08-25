#ifndef NODEITEMSTACKCONTAINER_H
#define NODEITEMSTACKCONTAINER_H

#include "nodeitemorderedcontainer.h"

class NodeItemStackContainer: public NodeItemOrderedContainer
{
public:
    NodeItemStackContainer(NodeViewItem *viewItem, NodeItemNew *parentItem);

    QPointF getFixedPositionForChild(NodeItemOrderedContainer *child);
    QPoint getIndexPositionForChild(NodeItemOrderedContainer *child);
};

#endif // NODEITEMSTACKCONTAINER_H
