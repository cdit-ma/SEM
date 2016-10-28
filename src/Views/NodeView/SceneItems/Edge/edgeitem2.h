#ifndef EDGEITEM2_H
#define EDGEITEM2_H

#include "../../../../Controllers/ViewController/edgeviewitem.h"
#include "../entityitem.h"

class NodeItem;
class EdgeItem2 : public EntityItem{
    Q_OBJECT

public:
    EdgeItem2(EdgeViewItem* edgeViewItem, NodeItem* parent, NodeItem* source, NodeItem* destination);
    ~EdgeItem2();

    void setPos(const QPointF &pos);
    QPointF getPos() const;

    QRectF boundingRect() const;
    QPointF getSceneEdgeTermination(bool left) const;
    QRectF currentRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private slots:
    void dataChanged(QString keyName, QVariant data);
    void dataRemoved(QString keyName);

private:


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

    QPointF src1;
    QPointF src2;
    QPointF dst1;
    QPointF dst2;
    QString text;


    NodeItem* vSrc;
    NodeItem* vDst;

    bool isCentered() const;

    void setCentered(bool centered);


    bool _isCentered;
    QPointF _centerPoint;
};



#endif // EDGEITEM2_H
