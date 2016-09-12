#ifndef EDGEITEMNEW_H
#define EDGEITEMNEW_H

#include "../edgeviewitem.h"

#include "entityitemnew.h"

class NodeItemNew;

class EdgeItemNew : public EntityItemNew{
    Q_OBJECT

public:
    EdgeItemNew(EdgeViewItem* edgeViewItem, NodeItemNew* parent, NodeItemNew* source, NodeItemNew* destination);
    ~EdgeItemNew();

    void setPos(const QPointF &pos);
    QPointF getPos() const;

    void setCenter(QPointF center);
    QPointF getCenter() const;

    QRectF boundingRect() const;
    QRectF currentRect() const;

private:
    QRectF translatedIconsRect() const;
    QRectF sourceIconRect() const;
    QRectF destinationIconRect() const;
    QRectF itemRect() const;

    QPolygonF sourceArrowHead() const;
    QPolygonF destinationArrowHead() const;
    QPolygonF triangle(QPointF startPoint, bool pointRight=true) const;

    NodeItemNew* getVisibleSource() const;
    NodeItemNew* getVisibleDestination() const;

    NodeItemNew* getParentNodeItem() const;
    NodeItemNew* getSourceItem();
    NodeItemNew* getDestinationItem();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QPointF getSourcePos(QPointF center = QPointF()) const;
    bool sourceExitsLeft(QPointF center = QPointF()) const;
    bool destinationEntersLeft(QPointF center = QPointF()) const;
    QPointF getDestinationPos(QPointF center = QPointF()) const;

    QPointF getInternalOffset() const;
    QPointF getArrowOffset(bool onLeft) const;

    QPointF getSceneEdgeTermination(bool left) const;

    void resetPosition();

    bool shouldReset();

    QPointF getSceneCenter() const;

public slots:
    void sourceParentVisibilityChanged();
    void destinationParentVisibilityChanged();

    void sourceParentMoved();
    void destinationParentMoved();

    void centerPointMoved();
private:
    void recalcSrcCurve(bool reset = false);
    void recalcDstCurve(bool reset = false);


    void setManuallyPositioned(bool value);
    bool hasSetPosition() const;

    bool _hasPosition;
    QPointF itemPos;

    EdgeViewItem* edgeViewItem;

    NodeItemNew* sourceItem;
    NodeItemNew* destinationItem;

    NodeItemNew* currentSrcItem;
    NodeItemNew* currentDstItem;

    QPainterPath sourceCurve;
    QPainterPath destinationCurve;



private slots:
    void dataChanged(QString keyName, QVariant data);
    void dataRemoved(QString keyName);

public:
    QRectF getElementRect(ELEMENT_RECT rect) const;
    QPainterPath getElementPath(ELEMENT_RECT rect) const;
    void setMoveStarted();
    bool setMoveFinished();

    // QGraphicsItem interface
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    // EntityItemNew interface
public:
    QPointF getTopLeftOffset() const;
};

#endif // EDGEITEMNEW_H
