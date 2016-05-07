#ifndef EDGEITEMARROW_H
#define EDGEITEMARROW_H
#include <QGraphicsPolygonItem>
#include "edgeitem.h"

class EdgeItemArrow : public QGraphicsPolygonItem
{
public:
    EdgeItemArrow(EdgeItem* parentEdge);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
private:
    EdgeItem* parentEdge;

    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // EDGEITEMARROW_H
