#ifndef ARROWLINE_H
#define ARROWLINE_H

#include <QGraphicsLineItem>
#include <QRectF>

class ArrowLine : public QGraphicsLineItem
{
public:
    ArrowLine() = default;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    void setHighlighted(bool highlight);
    void set_begin_point(QPointF point);
    void set_end_point(QPointF point);
    
    QPointF get_begin_point() const;
    QPointF get_end_point() const;
    
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
private:
    void update_head();
    
    QPolygonF arrow_head;
    QLineF line;
    
    int arrow_head_size = 3;
};

#endif