#ifndef NODEITEMSTACKCONTAINER_H
#define NODEITEMSTACKCONTAINER_H

#include "nodeitemorderedcontainer.h"

class StackContainerNodeItem: public ContainerNodeItem
{
    Q_OBJECT
public:
    StackContainerNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem);

    QPointF getStemAnchorPoint() const;
    QPointF getElementPosition(ContainerElementNodeItem *child);

    void childPosChanged();
};

#endif // NODEITEMSTACKCONTAINER_H
