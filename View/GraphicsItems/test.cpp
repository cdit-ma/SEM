#include "test.h"
#include <QGraphicsItem>
#include <QPainter>


Test::Test(QGraphicsItem *parent):QGraphicsItem(parent)
{

}

QRectF Test::boundingRect() const
{
    return QRectF(0,0,20,20);
}

void Test::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush(Qt::red);
    painter->drawRect(QRect(10,10,5,5));
    //painter->drawRect(boundingRect());
}
