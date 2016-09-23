#ifndef STACKNODEITEM_H
#define STACKNODEITEM_H

#include "basicnodeitem.h"

class StackNodeItem: public BasicNodeItem
{
    Q_OBJECT
public:
    StackNodeItem(NodeViewItem *viewItem, NodeItem *parentItem);

    QPointF getStemAnchorPoint() const;
    QPointF getElementPosition(BasicNodeItem *child);

    void childPosChanged();
};

#endif // STACKNODEITEM_H
