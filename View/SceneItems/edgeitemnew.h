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

    QRectF boundingRect() const;
    QRectF currentRect() const;

    QPointF getTopLeftOffset() const;
    QRectF getElementRect(ELEMENT_RECT rect) const;
    QPainterPath getElementPath(ELEMENT_RECT rect) const;
    void setMoveStarted();
    bool setMoveFinished();

private:
    QRectF itemRect() const;
    QRectF translatedItemRect() const;
    QRectF iconsRect() const;
    QRectF sourceIconRect() const;
    QRectF destinationIconRect() const;

    QPainterPath trianglePath(QPointF startPoint, bool pointRight=true) const;

    NodeItemNew* getVisibleSource() const;
    NodeItemNew* getVisibleDestination() const;

    NodeItemNew* getParentNodeItem() const;

    NodeItemNew* getSourceItem() const;
    NodeItemNew* getDestinationItem() const;

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

    NodeItemNew* sourceItem;
    NodeItemNew* destinationItem;

    NodeItemNew* currentSrcItem;
    NodeItemNew* currentDstItem;

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
