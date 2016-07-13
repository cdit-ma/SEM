#ifndef MODELITEMNEW_H
#define MODELITEMNEW_H

#include "nodeitemnew.h"

class ModelItemNew : public NodeItemNew
{
public:
    ModelItemNew(NodeViewItem *viewItem);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    QPainterPath getElementPath(ELEMENT_RECT rect) const;
    QRectF getCircleRect() const;
private:
    QColor backgroundColor;
};

#endif // MODELITEMNEW_H
