#ifndef STACKNODEITEM_H
#define STACKNODEITEM_H

#include "basicnodeitem.h"

class StackNodeItem: public BasicNodeItem
{
    Q_OBJECT
public:
    StackNodeItem(NodeViewItem *viewItem, NodeItem *parentItem, Qt::Orientation orientation = Qt::Vertical);

    QPointF getStemAnchorPoint() const;
    QPointF getElementPosition(BasicNodeItem *child);

    void childPosChanged();
private:
    Qt::Orientation orientation;
};

#endif // STACKNODEITEM_H
