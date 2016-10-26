#ifndef HARDWARENODEITEM_H
#define HARDWARENODEITEM_H

#include "stacknodeitem.h"

class HardwareNodeItem : public BasicNodeItem
{
public:
    HardwareNodeItem(NodeViewItem* viewItem, NodeItem* parentItem);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF getElementRect(ELEMENT_RECT rect) const;

    QPainterPath getElementPath(ELEMENT_RECT rect) const;
private:
     QRectF mainIconRect() const;
     QRectF mainRect() const;
     QRectF ipIconRect() const;
     QRectF ipTextRect() const;
     QRectF rightRect() const;
     QRectF labelRect() const;

private:
    void setupBrushes();

    QFont mainTextFont;
    int cornerRadius;
    int textHeight;
};

#endif // HARDWARENODEITEM_H
