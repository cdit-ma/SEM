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

    bool isCentered() const;
    void resetCenter();
    void setCentered(bool centered);
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private slots:
    void dataChanged(QString keyName, QVariant data);
    void dataRemoved(QString keyName);
    void updateEdge();
    void srcAncestorVisibilityChanged();
    void dstAncestorVisibilityChanged();
private:
    //Rectangle helpers
    QRectF srcIconRect() const;
    QRectF centerIconRect() const;
    QRectF dstIconRect() const;

    QRectF srcIconCircle() const;
    QRectF dstIconCircle() const;

    QRectF centerCircleRect() const;
    QRectF sceneCenterCircleRect() const;
    QRectF translatedCenterCircleRect(QPointF center = QPointF()) const;

    QPointF getCenterCircleTermination(bool left, QPointF center = QPointF()) const;


    bool srcExitsLeft(QPointF center = QPointF()) const;
    bool dstExitsLeft(QPointF center = QPointF()) const;
    bool srcLeftOfDst() const;


    void updateSrcCurve(QPointF srcP, QPointF ctrP, bool srcP_Left, bool ctrP_Left);
    void updateDstCurve(QPointF dstP, QPointF ctrP, bool dstP_Left, bool ctrP_Left);

    QPainterPath calculateArrowHead(QPointF startPoint, bool pointLeft) const;
    QPainterPath calculateBezierCurve(QPointF P1, QPointF P2, bool P1_Left, bool P2_Left) const;


    NodeItem* getFirstVisibleParent(NodeItem* item);


    NodeItem* src;
    NodeItem* dst;
    NodeItem* vSrc;
    NodeItem* vDst;


    QPainterPath srcCurve;
    QPainterPath dstCurve;
    QPainterPath srcArrow;
    QPainterPath dstArrow;

    bool srcCurveEntersCenterLeft;

    QMarginsF margins;

    bool _isCentered;
    QPointF _centerPoint;
};



#endif // EDGEITEM_H
