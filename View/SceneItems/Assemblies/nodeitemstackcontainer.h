#ifndef NODEITEMSTACKCONTAINER_H
#define NODEITEMSTACKCONTAINER_H

#include "nodeitemorderedcontainer.h"

class StackContainerNodeItem: public ContainerNodeItem
{
public:
    StackContainerNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem);

    QPointF getStemAnchorPoint() const;
    QPointF getElementPosition(ContainerElementNodeItem *child);
    QPoint getElementIndex(ContainerElementNodeItem *child);

    void childPosChanged();

private:
    QVector<QLineF> leaves;
    QPen leafPen;

    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // NODEITEMSTACKCONTAINER_H
