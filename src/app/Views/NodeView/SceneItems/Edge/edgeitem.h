#ifndef EDGEITEM_H
#define EDGEITEM_H

#include "../../../../Controllers/ViewController/edgeviewitem.h"
#include "../entityitem.h"

#include <QPolygonF>

class NodeItem;
class EdgeItem : public EntityItem{
    Q_OBJECT

public:
    EdgeItem(EdgeViewItem* edgeViewItem, NodeItem* parent, NodeItem* source, NodeItem* destination);
    ~EdgeItem();

    void setPos(const QPointF &pos);
    QPointF getPos() const;

    QRectF boundingRect() const;
    QRectF edgeRect() const;
    QPointF getSceneEdgeTermination(EDGE_DIRECTION direction, EDGE_KIND kind) const;
    QRectF currentRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF getElementRect(EntityRect rect) const;
    QPainterPath getElementPath(EntityRect rect) const;

    bool setMoveFinished();

    bool isCentered() const;
    void resetCenter();
    void setCentered(bool centered);
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private slots:
    void dataChanged(const QString& key_name, const QVariant& data);
    void dataRemoved(const QString& key_name);
    void updateEdge();
    void srcAncestorVisibilityChanged();
    void dstAncestorVisibilityChanged();
    void repaint();
private:
    void updateEdgeOpacity();
    void sourceIconHover(bool handle, const QPointF& pos);
    void targetIconHover(bool handle, const QPointF& pos);
    void moveHover(bool handle, const QPointF& pos);

    //Rectangle helpers
    QRectF srcIconRect() const;
    QRectF centerIconRect() const;
    QRectF dstIconRect() const;

    QRectF srcIconCircle() const;
    QRectF dstIconCircle() const;

    QRectF centerCircleRect() const;
    QRectF translatedCenterCircleRect(QPointF center = QPointF()) const;

    QPointF getCenterCircleTermination(bool left, QPointF center = QPointF()) const;


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

    qreal inactive_opacity_ = 0.50;

    QMarginsF margins;
    EDGE_KIND kind;
    bool _isCentered;
    QPointF _centerPoint;

    // EntityItem interface
public:
    void updateZValue(bool childSelected, bool childActive);
};



#endif // EDGEITEM_H
