#ifndef MANAGEMENTCOMPONENTNODEITEM_H
#define MANAGEMENTCOMPONENTNODEITEM_H


#include "../nodeitemnew.h"
#include <QDebug>

class ManagementComponentNodeItem : public NodeItemNew
{
public:
    ManagementComponentNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QPainterPath getElementPath(EntityItemNew::ELEMENT_RECT rect) const;

    QRectF getElementRect(ELEMENT_RECT rect) const;

    QRectF mainRect() const;

    QRectF mainIconRect() const;

    QRectF loggerIconRect() const;

    QRectF nameTextRect() const;

    QRectF rightRect() const;

    QRectF labelRect() const;

    QPolygonF rightPoly() const;

    QPolygonF leftPoly() const;


private:
    QFont mainTextFont;
    int textHeight;
};

#endif // MANAGEMENTCOMPONENTNODEITEM_H
