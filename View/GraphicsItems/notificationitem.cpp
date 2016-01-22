#include "notificationitem.h"
#include "graphmlitem.h"
#include "../nodeview.h"

#include <QPainter>
#define RADIUS 10
#define VERT_PADDING 4
#define HORIZ_PADDING 4

NotificationItem::NotificationItem(GraphMLItem *item):QGraphicsObject(item)
{
    this->item = item;
    setErrorType(ET_OKAY);
}

void NotificationItem::setErrorType(ERROR_TYPE errorType)
{
    this->errorType = errorType;
    switch(errorType){
    case ET_OKAY:
        setVisible(false);
        return;
    case ET_CRITICAL:
        setVisible(true);
        setBackgroundColor(Qt::red);
        break;
    case ET_WARNING:
        setVisible(true);
        setBackgroundColor(QColor(255,140,0));
        break;
    }
    update();
}

void NotificationItem::setBackgroundColor(QColor color)
{
    backgroundColor = color;
}

QRectF NotificationItem::boundingRect() const
{
    return QRectF(0, 0, 2 * RADIUS, 3 * RADIUS);
}

void NotificationItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setWidthF(1);

     pen.setJoinStyle(Qt::MiterJoin);

    QPainterPath path;
    QPointF bottomMid;
    bottomMid = boundingRect().bottomRight() - QPointF(boundingRect().width()/2,0);


    path.moveTo(QPointF(0,RADIUS));

    path.lineTo(bottomMid);
    path.lineTo(2*RADIUS, 1 * RADIUS);
    path.moveTo(QPointF(RADIUS,RADIUS));
    path.arcTo(QRectF(0,0, 2*RADIUS, 2*RADIUS),180, -180);
    path = path.simplified();

    painter->setPen(pen);
    painter->setBrush(backgroundColor);
    painter->drawPath(path);

    QRectF imageRect;
    imageRect.setWidth((2.0/3.0) * 2*RADIUS);
    imageRect.setHeight(imageRect.width());
    imageRect.moveCenter(QPointF(RADIUS, RADIUS));

    QPixmap imageData;
    if(errorType == ET_CRITICAL){
        imageData = item->getNodeView()->getImage("Actions", "Critical");
    }else if(errorType == ET_WARNING){
        imageData = item->getNodeView()->getImage("Actions", "Exclamation");
    }

    painter->drawPixmap(imageRect.toAlignedRect(), imageData);
}

