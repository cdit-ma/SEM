#ifndef NODEITEMCOLUMNCONTAINER_H
#define NODEITEMCOLUMNCONTAINER_H

#include "../nodeitemcontainer.h"
#include "nodeitemorderedcontainer.h"

class ColumnContainerNodeItem : public ContainerNodeItem
{
    Q_OBJECT
public:
    ColumnContainerNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPointF getElementPosition(ContainerElementNodeItem *child);
    QPoint getElementIndex(ContainerElementNodeItem *child);

private:
    QPointF getColumnPosition(QPoint index) const;
    QRectF getColumnRect(int x);
    int columnWidth;
    int columnSpacing;
    int columnHeight;
};

#endif // NODEITEMCOLUMNCONTAINER_H
