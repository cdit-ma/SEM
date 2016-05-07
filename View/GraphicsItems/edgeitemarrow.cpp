#include "edgeitemarrow.h"
#include <QGraphicsSceneMouseEvent>
#include "../../enumerations.h"
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

void EdgeItemArrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    //Get Render State.
    GraphMLItem::RENDER_STATE renderState = parentEdge->getRenderStateFromZoom(lod);

    if(renderState >= GraphMLItem::RS_MINIMAL){
        QGraphicsPolygonItem::paint(painter, option, widget);
    }
}
