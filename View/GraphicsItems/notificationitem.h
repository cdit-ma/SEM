#ifndef NOTIFICATIONITEM_H
#define NOTIFICATIONITEM_H
#include <QGraphicsObject>
#include "../../enumerations.h"
class GraphMLItem;
class NotificationItem : public QGraphicsObject
{
public:
    NotificationItem(GraphMLItem* parent);

    void setErrorType(ERROR_TYPE errorType, QString tooltip="");

    qreal getAngle();

    void setBackgroundColor(QColor color);
    // QGraphicsItem interface
public:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QPixmap pixmap;
    QColor backgroundColor;
    GraphMLItem* item;
    ERROR_TYPE errorType;
};

#endif // NOTIFICATIONITEM_H
