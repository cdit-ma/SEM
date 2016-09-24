#ifndef MANAGEMENTCOMPONENTNODEITEM_H
#define MANAGEMENTCOMPONENTNODEITEM_H

#include "nodeitem.h"
#include <QDebug>

class ManagementComponentNodeItem : public NodeItem
{
public:
    ManagementComponentNodeItem(NodeViewItem *viewItem, NodeItem *parentItem);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QPainterPath getElementPath(EntityItem::ELEMENT_RECT rect) const;

    QRectF getElementRect(ELEMENT_RECT rect) const;

private:
    QRectF mainRect() const;

    QRectF mainIconRect() const;

    QRectF subIconRect() const;

    QRectF secondSubIconRect() const;

    QRectF thirdSubIconRect() const;

    QRectF rightRect() const;

    QRectF labelRect() const;

    QPolygonF getRightPoly() const;

    QPolygonF getLeftPoly() const;

private:
    QFont mainTextFont;
    QPolygonF leftPoly;
    QPolygonF rightPolyExpanded;
    void setupPolys();

    bool isDeployed();
};

#endif // MANAGEMENTCOMPONENTNODEITEM_H
