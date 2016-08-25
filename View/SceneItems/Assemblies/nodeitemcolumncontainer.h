#ifndef NODEITEMCOLUMNCONTAINER_H
#define NODEITEMCOLUMNCONTAINER_H

#include "../nodeitemcontainer.h"
class NodeItemColumnItem;
class NodeItemColumnContainer : public NodeItemContainer
{
    Q_OBJECT
public:
    NodeItemColumnContainer(NodeViewItem *viewItem, NodeItemNew *parentItem);

    void setColumnCount(int count);
    QPointF getColumnPosition(int x, int y);
    QRectF getColumnRect(int x);
    QPointF getPositionForChild(NodeItemNew* child);

    QPoint getIndexForChild(NodeItemColumnItem* child);
private:
    int getColumnCount();
    int columnCount;
    int columnWidth;
    int columnSpacing;
    int columnHeight;


    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // NODEITEMCOLUMNCONTAINER_H
