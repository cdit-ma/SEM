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

    QMap<int, QMap<int, QSizeF> > GetGridRectangles(const QList<NodeItem*> &children, int ignore_indexes_higher_than = -1);
    QPointF GetGridAlignedTopLeft() const;

    void childPosChanged(EntityItem* child);
    void ChildIndexChanged(NodeItem* child);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    int GetHorizontalGap() const;
    int GetVerticalGap() const;
    Qt::Orientation orientation;

    QMap<int, QMap<int, QSizeF> > cached_grid_rectangles;
    QMap<int, QMultiMap<int, NodeItem*> > cached_node_items;

    StaticTextItem* left_text_item = 0;
    StaticTextItem* right_text_item = 0;
    StaticTextItem* bottom_text_item = 0;

};

#endif // STACKNODEITEM_H
