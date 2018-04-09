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

    void SetPaintSubArea(int row, int col, QColor background_color);
    void SetSubAreaLabel(int row, int col, QString label, QColor background_color);
    void SetSubAreaIcons(int row, int col, QString icon_path, QString icon_name);


    void childPosChanged(EntityItem* child);
    void ChildIndexChanged(NodeItem* child);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    int GetHorizontalGap() const;
    int GetVerticalGap() const;
    Qt::Orientation orientation;

    void recalculateSubAreas();

    QMap<int, QMap<int, QSizeF> > cached_grid_rectangles;
    QMap<int, QMultiMap<int, NodeItem*> > cached_node_items;
    
    bool sub_areas_dirty = true;

    QList<QPair <QPair<int, int>, QColor> > sub_areas_on;

    QMap<int, QMultiMap<int, QPair<QString, QString> > > gap_icon_path;
    QMap<int, QMultiMap<int, QRectF> > gap_icon_rects;

    QMap<int, QMap<int, QRectF> > sub_area_rects;
    QMap<int, QMap<int, StaticTextItem*> > sub_area_labels;
    

    StaticTextItem* left_text_item = 0;
    StaticTextItem* right_text_item = 0;
    StaticTextItem* bottom_text_item = 0;

};

#endif // STACKNODEITEM_H
