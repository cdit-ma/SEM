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
    QPointF getSceneEdgeTermination(bool left) const;
    QRectF currentRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF getElementRect(ELEMENT_RECT rect) const;
    QPainterPath getElementPath(ELEMENT_RECT rect) const;

    void setMoveStarted();
    bool setMoveFinished();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private slots:
    void dataChanged(QString keyName, QVariant data);
    void dataRemoved(QString keyName);

private:
    QPainterPath trianglePath(QPointF startPoint, bool pointRight=true) const;

    QRectF srcIconRect() const;
    QRectF centerIconRect() const;
    QRectF dstIconRect() const;

    QRectF srcIconCircle() const;
    QRectF dstIconCircle() const;


    QPointF getCenterCircleTermination(bool left, QPointF center = QPointF()) const;
    QRectF centerCircleRect() const;
    QRectF translatedCenterCircleRect(QPointF center = QPointF()) const;
    QRectF sceneCenterCircleRect() const;


    bool srcExitsLeft(QPointF center = QPointF()) const;
    bool dstExitsLeft(QPointF center = QPointF()) const;
    bool srcLeftOfDst() const;


    void srcParentVisibilityChanged();
    void dstParentVisibilityChanged();

    void srcParentMoved();
    void dstParentMoved();
    void centerMoved();

    void recalcSrcCurve();
    void recalcDstCurve();
    void recalculateEdgeDirections();

    void resetCenter();

    NodeItem* getFirstVisibleParent(NodeItem* item);



    NodeItem* src;
    NodeItem* dst;


    QPainterPath sourceCurve;
    QPainterPath destinationCurve;
    QPainterPath sourceArrow;
    QPainterPath destinationArrow;



    QPointF srcCurveP1;
    QPointF srcCurveP2;
    bool srcCurveP1Left;
    bool srcCurveP2Left;

    QPointF dstCurveP1;
    QPointF dstCurveP2;
    bool dstCurveP1Left;
    bool dstCurveP2Left;


    bool dstLeft;

    QMarginsF margins;
    NodeItem* vSrc;
    NodeItem* vDst;

    bool isCentered() const;

    void setCentered(bool centered);


    bool _isCentered;
    QPointF _centerPoint;
};



#endif // EDGEITEM_H
