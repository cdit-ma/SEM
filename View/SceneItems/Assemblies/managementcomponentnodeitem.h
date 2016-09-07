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
    int textHeight;
    QPolygonF leftPoly;
    QPolygonF rightPolyExpanded;
    void setupPolys();

    bool isLogger();
    bool isDeployed();
};

#endif // MANAGEMENTCOMPONENTNODEITEM_H
