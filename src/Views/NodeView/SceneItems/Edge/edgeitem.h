#ifndef EDGEITEM_H
#define EDGEITEM_H

#include "../../../../Controllers/ViewController/edgeviewitem.h"
#include "../entityitem.h"

class NodeItem;
class EdgeItem : public EntityItem{
    Q_OBJECT

public:
    EdgeItem(EdgeViewItem* edgeViewItem, NodeItem* parent, NodeItem* source, NodeItem* destination);
    ~EdgeItem();

    void setPos(const QPointF &pos);
    QPointF getPos() const;

    QRectF boundingRect() const;
    QRectF currentRect() const;

    QPointF getTopLeftOffset() const;
    QRectF getElementRect(ELEMENT_RECT rect) const;
    QPainterPath getElementPath(ELEMENT_RECT rect) const;
    void setMoveStarted();
    bool setMoveFinished();

private:
    QRectF centerRect() const;
    QRectF translatedCenterRect() const;

    QRectF centerIconRect() const;

    QRectF sourceIconRect() const;
    QRectF destinationIconRect() const;

    QRectF sourceIconCircle() const;
    QRectF destinationIconCircle() const;

    QPainterPath trianglePath(QPointF startPoint, bool pointRight=true) const;

    NodeItem* getVisibleSource() const;
    NodeItem* getVisibleDestination() const;

    NodeItem* getParentNodeItem() const;

    NodeItem* getSourceItem() const;
    NodeItem* getDestinationItem() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QPointF getSourcePos(QPointF center = QPointF()) const;
    bool sourceExitsLeft(QPointF center = QPointF()) const;

    QPointF getDestinationPos(QPointF center = QPointF()) const;
    bool destinationEntersLeft(QPointF center = QPointF()) const;

    QPointF getSceneEdgeTermination(bool left) const;

    void resetPosition();


    QPointF getSceneCenter() const;

private slots:
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

    NodeItem* sourceItem;
    NodeItem* destinationItem;

    NodeItem* currentSrcItem;
    NodeItem* currentDstItem;

    QPainterPath sourceCurve;
    QPainterPath destinationCurve;
    QPainterPath sourceArrow;
    QPainterPath destinationArrow;

    QMarginsF margins;



private slots:
    void dataChanged(QString keyName, QVariant data);
    void dataRemoved(QString keyName);

public:

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

};

#endif // EDGEITEMNEW_H
