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

    void setPos(const QPointF &pos);
    QPointF getPos() const;

    void setCenter(QPointF center);
    QPointF getCenter() const;

    QRectF boundingRect() const;
    QRectF currentRect() const;

private:
    QRectF smallRect() const;
    QRectF leftRect() const;
    QRectF centerRect() const;
    QRectF rightRect() const;
    QRectF handleRect() const;
    QPolygonF triangle() const;

    NodeItemNew* getVisibleSource();
    NodeItemNew* getVisibleDestination();

    NodeItemNew* getParentItem();
    NodeItemNew* getSourceItem();
    NodeItemNew* getDestinationItem();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QPointF getSourcePos() const;
    QPointF getDestinationPos() const;

    QPointF getCenterOffset() const;
    QPointF getInternalOffset() const;

    QPointF getSceneEdgeTermination(bool left) const;

    void resetPosition();

    bool shouldReset();


public slots:
    void sourceHidden();
    void destinationHidden();
    void sourceMoved();
    void destinationMoved();
    void centerMoved();
private:
    void recalcSrcCurve(bool reset = false);
    void recalcDstCurve(bool reset = false);


    void setManuallyPositioned(bool value);
    bool hasSetPosition();

    bool _hasPosition;

    KIND edge_kind;
    EdgeViewItem* edgeViewItem;
    NodeItemNew* parentItem;
    NodeItemNew* sourceItem;
    NodeItemNew* destinationItem;

    NodeItemNew* vSrcItem;
    NodeItemNew* vDstItem;

    QPainterPath sourceCurve;
    QPainterPath destinationCurve;

    QPointF itemPos;


private slots:
    void dataChanged(QString keyName, QVariant data);
    void dataRemoved(QString keyName);

    // EntityItemNew interface
public:
    QPointF validateAdjustPos(QPointF delta);

    // EntityItemNew interface
public:
    void setMoving(bool moving);

    // QGraphicsItem interface
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    // EntityItemNew interface
public:
};

#endif // EDGEITEMNEW_H
