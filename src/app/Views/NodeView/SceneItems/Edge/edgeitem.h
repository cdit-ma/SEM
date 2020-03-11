#ifndef EDGEITEM_H
#define EDGEITEM_H

#include "../../../../Controllers/ViewController/edgeviewitem.h"
#include "../entityitem.h"

class NodeItem;
class EdgeItem : public EntityItem
{
    Q_OBJECT

public:
    EdgeItem(EdgeViewItem* edgeViewItem, NodeItem* parent, NodeItem* source, NodeItem* destination);
    ~EdgeItem() final;

    void setPos(const QPointF &pos) final;
    QPointF getPos() const final;

    QRectF boundingRect() const final;
    QRectF edgeRect() const;
    QRectF currentRect() const final;
    
    QPointF getSceneEdgeTermination(EDGE_DIRECTION direction, EDGE_KIND kind) const final;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) final;
    QRectF getElementRect(EntityRect rect) const final;
    QPainterPath getElementPath(EntityRect rect) const final;

    bool setMoveFinished() final;

    bool isCentered() const;
    void resetCenter();
    void setCentered(bool centered);
    
    void updateZValue(bool childSelected, bool childActive) final;

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) final;

private slots:
    void dataChanged(const QString& key_name, const QVariant& data) final;
    void dataRemoved(const QString& key_name) final;
    
    void srcAncestorVisibilityChanged();
    void dstAncestorVisibilityChanged();
    
    void updateEdge();
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

    NodeItem* src = nullptr;
    NodeItem* dst = nullptr;
    NodeItem* vSrc = nullptr;
    NodeItem* vDst = nullptr;

    QPainterPath srcCurve;
    QPainterPath dstCurve;
    QPainterPath srcArrow;
    QPainterPath dstArrow;
    
    bool srcCurveEntersCenterLeft = false;
    bool _isCentered = true;
    
    EDGE_KIND kind;
    
    qreal inactive_opacity_ = 0.50;
    QPointF _centerPoint;
    QMarginsF margins;
};

#endif // EDGEITEM_H
