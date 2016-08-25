#ifndef NODEITEMCOLUMNCONTAINER_H
#define NODEITEMCOLUMNCONTAINER_H

#include "../nodeitemcontainer.h"
#include "nodeitemorderedcontainer.h"

class NodeItemColumnContainer : public NodeItemOrderedContainer
{
    Q_OBJECT
public:
    NodeItemColumnContainer(NodeViewItem *viewItem, NodeItemNew *parentItem);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPointF getFixedPositionForChild(NodeItemOrderedContainer *child);
    QPoint getIndexPositionForChild(NodeItemOrderedContainer *child);

private:
    QPointF getColumnPosition(QPoint index) const;
    QRectF getColumnRect(int x);
    int columnWidth;
    int columnSpacing;
    int columnHeight;
};

#endif // NODEITEMCOLUMNCONTAINER_H
