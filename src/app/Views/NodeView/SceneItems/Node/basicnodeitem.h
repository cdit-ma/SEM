#ifndef BASICNODEITEM_H
#define BASICNODEITEM_H

#include "nodeitem.h"

class BasicNodeItem : public NodeItem
{
    Q_OBJECT
public:
    BasicNodeItem(NodeViewItem* viewItem, NodeItem* parentItem);

    bool isSortOrdered() const;
    void setSortOrdered(bool ordered);
    QRectF bodyRect() const;
    //Pure virtual functions.
    virtual QPointF getElementPosition(BasicNodeItem* child);

    BasicNodeItem* getParentContainer() const;


    void setPos(const QPointF &pos);
    QPointF getNearestGridPoint(QPointF newPos);

    

    // QGraphicsItem interface
public:

    QRectF getElementRect(EntityRect rect) const;
    QPainterPath getElementPath(EntityRect rect) const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void paintBackground(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QPointF validateMove(QPointF delta);
protected:
    QRectF headerRect() const;
private:
    QRectF connectSourceRect() const;
    QRectF connectTargetRect() const;

    QRectF headerContent() const;
    
    QRectF headerContent_Icon() const;
    QRectF headerContent_Icon_Overlay() const;

    QRectF headerContent_Data() const;
    QRectF headerContent_Data_Primary() const;
    QRectF headerContent_Data_Secondary() const;

    QRectF headerContent_Data_Primary_Text() const;
    QRectF headerContent_Data_Primary_Icon() const;
    QRectF headerContent_Data_Secondary_Text() const;
    QRectF headerContent_Data_Secondary_Icon() const;

    QRectF expandedStateRect() const;

    BasicNodeItem* parentContainer = 0;
    bool is_sorted_ordered = false;

    QMarginsF header_margins;

    const qreal ratio = 4.0 / 7.0;
};

#endif // BASICNODEITEM_H
