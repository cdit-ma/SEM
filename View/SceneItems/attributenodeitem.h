#ifndef ATTRIBUTENODEITEM_H
#define ATTRIBUTENODEITEM_H

#include "nodeitemnew.h"



class attributenodeitem : public NodeItemNew
{
public:
    attributenodeitem();


private:
    qreal height;
    qreal width;

    QFont mainTextFont;

    QRectF mainIconRect() const;


    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // EntityItemNew interface
public:
    QRectF getElementRect(ELEMENT_RECT rect) const;
    QPainterPath getElementPath(ELEMENT_RECT rect) const;
};

#endif // ATTRIBUTENODEITEM_H
