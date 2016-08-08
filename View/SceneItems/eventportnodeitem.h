#ifndef EVENTPORTNODEITEM_H
#define EVENTPORTNODEITEM_H

#include "nodeitemnew.h"

class EventPortNodeItem : public NodeItemNew
{
public:
    EventPortNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QRectF getElementRect(EntityItemNew::ELEMENT_RECT rect) const;

    QPainterPath getElementPath(EntityItemNew::ELEMENT_RECT rect) const;


private:
    QFont mainTextFont;

    bool isOutEventPort();
    bool isInEventPort();
    bool isDelegate();

    QRectF mainRect() const;

    QRectF labelRect() const;

    QRectF labelBGRect() const;

    QRectF mainIconRect() const;

    QPolygonF mainIconPoly() const;

    QRectF subIconRect() const;

    QPolygonF subIconPoly() const;

    void initPolys();

    qreal width;
    qreal height;

    QPolygonF leftIconPoly;
    QPolygonF rightIconPoly;

    QPolygonF rightSubPoly;
    QPolygonF leftSubPoly;

    bool iconRight;


    // EntityItemNew interface
private slots:
    void dataChanged(QString keyName, QVariant data);
};

#endif // EVENTPORTNODEITEM_H
