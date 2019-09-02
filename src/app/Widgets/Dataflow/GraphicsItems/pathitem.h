#ifndef PATHITEM_H
#define PATHITEM_H

#include <QGraphicsPathItem>

namespace MEDEA {

class PathItem : public QGraphicsPathItem
{
public:
    PathItem(const QPainterPath &path, QGraphicsItem* parent = nullptr);

    // QGraphicsItem interface
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

}

#endif // PATHITEM_H
