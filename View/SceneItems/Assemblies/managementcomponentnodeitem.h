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

    QRectF subIconRect() const;

    QRectF secondSubIconRect() const;

    QRectF thirdSubIconRect() const;

    QRectF rightRectExpanded() const;

    QRectF rightRect() const;

    QRectF labelRect() const;

    QPolygonF getRightPolyExpanded() const;

    QPolygonF getRightPoly() const;

    QPolygonF getLeftPoly() const;

    QRectF expandStateRect() const;


private:
    QFont mainTextFont;
    int textHeight;
    QPolygonF leftPoly;
    QPolygonF rightPoly;
    QPolygonF rightPolyExpanded;
    void setupPolys();

    bool isLogger();
    bool isDeployed();
};

#endif // MANAGEMENTCOMPONENTNODEITEM_H
