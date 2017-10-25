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
    virtual QPoint getElementIndex(BasicNodeItem* child);

    BasicNodeItem* getParentContainer() const;

    QPoint getIndexPosition() const;
    void setIndexPosition(QPoint point);

    void dataChanged(QString keyName, QVariant data);

    void setPos(const QPointF &pos);
    QPointF getNearestGridPoint(QPointF newPos);

    // QGraphicsItem interface
public:
    QRectF getElementRect(ELEMENT_RECT rect) const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QRectF headerRect() const;
    QRectF connectRect() const;
    QRectF connectSourceRect() const;
    QRectF connectTargetRect() const;
    QRectF edgeKindRect() const;
    QRectF innerHeaderRect() const;

    QRectF headerTextRect() const;
    QRectF tertiaryIconRect() const;
    QRectF iconRect() const;
    QRectF iconOverlayRect() const;

    QRectF topTextRect() const;
    QRectF bottomTextRect() const;
    QRectF bottomTextOutlineRect() const;

    QRectF bottomIconRect() const;
    QRectF bottomRect() const;

    QRectF deployedRect() const;
    QRectF lockedRect() const;
    QRectF qosRect() const;
    QRectF expandStateRect() const;

    BasicNodeItem* parentContainer;
    bool _isSortOrdered;
    QMarginsF headerMargin;
    QPoint indexPosition;
};

#endif // BASICNODEITEM_H
