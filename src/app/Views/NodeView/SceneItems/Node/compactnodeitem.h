#ifndef COMPACTNODEITEM_H
#define COMPACTNODEITEM_H

#include "basicnodeitem.h"

class CompactNodeItem : public BasicNodeItem
{
    Q_OBJECT
public:
    CompactNodeItem(NodeViewItem* viewItem, NodeItem* parentItem);
public:
    QRectF getElementRect(EntityRect rect) const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    QRectF innerHeaderRect() const;
    QRectF textRect_Top() const;
    QRectF textRect_Bottom() const;
    
    QRectF topRect() const;
    QRectF bottomRect() const;

    QRectF iconRect_Top() const;
    QRectF iconRect_Bottom() const;

    QMarginsF header_margins;
};

#endif // COMPACTNODEITEM_H
