#include "edgeitemarrow.h"
#include <QGraphicsSceneMouseEvent>
EdgeItemArrow::EdgeItemArrow(EdgeItem *parentEdge):QGraphicsPolygonItem(parentEdge)
{
    this->parentEdge = parentEdge;
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemStacksBehindParent, true);
}

void EdgeItemArrow::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        bool controlPressed = event->modifiers().testFlag(Qt::ControlModifier);
        parentEdge->handleSelection(true, controlPressed);
    }
}


void EdgeItemArrow::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
    parentEdge->handleHover(true);
}

void EdgeItemArrow::hoverMoveEvent(QGraphicsSceneHoverEvent*)
{
    parentEdge->handleHover(true);
}

void EdgeItemArrow::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    parentEdge->handleHover(false);
}
