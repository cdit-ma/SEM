#ifndef EDGEITEMNEW_H
#define EDGEITEMNEW_H

#include "../edgeviewitem.h"

#include "entityitemnew.h"

class NodeItemNew;

class EdgeItemNew : public EntityItemNew{
    Q_OBJECT

public:
    enum KIND{DEFAULT};

    EdgeItemNew(EdgeViewItem* edgeViewItem, NodeItemNew* parent, NodeItemNew* source, NodeItemNew* destination, KIND edge_kind = DEFAULT);
    ~EdgeItemNew();
    KIND getEdgeItemKind();

    QRectF smallRect() const;
    QRectF leftRect() const;
    QRectF centerRect() const;
    QRectF rightRect() const;

    NodeItemNew* getParentItem();
    NodeItemNew* getSourceItem();
    NodeItemNew* getDestinationItem();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF getElementRect(ELEMENT_RECT rect) const;
    QRectF currentRect() const;
    QRectF boundingRect() const;

    QRectF translatedRect() const;

    QPointF getSourcePos() const;
    QPointF getDestinationPos() const;

    QPointF getCenterOffset() const;

    void resetPosition();

    bool shouldReset();


public slots:
    void sourceMoved();
    void destinationMoved();
    void centerMoved();
private:
    void recalcSrcCurve(bool reset = false);
    void recalcDstCurve(bool reset = false);


    bool manuallySet;
    KIND edge_kind;
    EdgeViewItem* edgeViewItem;
    NodeItemNew* parentItem;
    NodeItemNew* sourceItem;
    NodeItemNew* destinationItem;

    QPainterPath sourceCurve;
    QPainterPath destinationCurve;


private slots:
    void dataChanged(QString keyName, QVariant data);

    // EntityItemNew interface
public:
    QPointF validateAdjustPos(QPointF delta);

    // EntityItemNew interface
public:
    void setMoving(bool moving);

    // QGraphicsItem interface
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
};

#endif // EDGEITEMNEW_H
