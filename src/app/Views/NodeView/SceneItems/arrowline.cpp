#include "arrowline.h"
#include "../../../theme.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <QPainter>

QRectF ArrowLine::boundingRect() const
{
    qreal pen_width = pen().width() / 2.0;
    auto rect = QRectF(line.p1(), line.p2()).normalized();
    
    //Adjust for the pen_width
    rect = rect.adjusted(-pen_width, -pen_width, pen_width, pen_width);
    rect = rect.united(arrow_head.boundingRect());
    return rect;
}

void ArrowLine::setHighlighted(bool highlight)
{
    auto pen_ = pen();
    pen_.setStyle(highlight ? Qt::SolidLine : Qt::DotLine);
    pen_.setWidthF(highlight ? 1 : 0.5);
    setPen(pen_);
    update();
}

QPainterPath ArrowLine::shape() const
{
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrow_head);
    return path;
}

void ArrowLine::set_begin_point(QPointF point)
{
    if(point != line.p1()){
        line.setP1(point);
        update_head();
    }
}

QPointF ArrowLine::get_begin_point() const
{
    return line.p1();
}

QPointF ArrowLine::get_end_point() const
{
    return line.p2();
}

void ArrowLine::set_end_point(QPointF point)
{
    if(point != line.p2()){
        line.setP2(point);
        update_head();
    }
}

void ArrowLine::update_head()
{
    setLine(line);

    auto angle = ::acos(line.dx() / line.length());
    if(line.dy() >= 0){
        angle = (M_PI * 2) - angle;
    }

    auto arrow_p1 = line.p1() + QPointF(sin(angle + M_PI / 3) * arrow_head_size, cos(angle + M_PI / 3) * arrow_head_size);
    auto arrow_p2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrow_head_size, cos(angle + M_PI - M_PI / 3) * arrow_head_size);    
    arrow_head.clear();
    arrow_head << line.p1() << arrow_p1 << arrow_p2;
    update();
}

void ArrowLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    auto pen_ = pen();
    if(!line.isNull()){
        painter->setPen(pen_);
        painter->drawLine(line);
        pen_.setStyle(Qt::SolidLine);
        painter->setPen(pen_);
        painter->setBrush(pen_.color());

        if(line.length() > 5){
            painter->drawPolygon(arrow_head);
        }
    }
}