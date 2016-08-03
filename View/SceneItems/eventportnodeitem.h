#ifndef EVENTPORTNODEITEM_H
#define EVENTPORTNODEITEM_H

#include "nodeitemnew.h"

class EventPortNodeItem : public NodeItemNew
{
public:
    EventPortNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QPainterPath getElementPath(EntityItemNew::ELEMENT_RECT rect) const;

private:
    QFont mainTextFont;

    bool isOutEventPort();
    bool isInEventPort();
    bool isDelegate();

    QRectF mainRect();

    QRectF labelRect();

    qreal width;
    qreal height;


};

#endif // EVENTPORTNODEITEM_H
