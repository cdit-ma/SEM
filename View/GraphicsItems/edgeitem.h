#ifndef NODECONNECTION_H
#define NODECONNECTION_H

#include <QGraphicsItem>
#include <QObject>

#include "nodeitem.h"
#include "../../Model/node.h"
#include "../../Model/edge.h"
#include "../../Model/graphmldata.h"


class EdgeItem: public GraphMLItem
{
    Q_OBJECT

public:
    EdgeItem(Edge *edge, NodeItem* s, NodeItem* d);
    ~EdgeItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    NodeItem* getSource();
    NodeItem* getDestination();


public slots:
    void setOpacity(qreal opacity);

    void setSelected(bool selected);
    void setVisible(bool visible);

    void updateEdge();
    void graphMLDataChanged(GraphMLData * data);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    void setLabelFont();
    void updateLabel();
    void updateLines();

    void setupBrushes();
    void forceVisible(bool visible);

    QPainterPath* painterPath;

    QVector<QGraphicsLineItem*> lineItems;
    QVector<QGraphicsLineItem*> arrowHeadLines;

    NodeItem* sourceParent;
    NodeItem* destinationParent;
    NodeItem* source;
    NodeItem* destination;


    bool IS_SELECTED;
    bool IS_INSTANCE_LINK;
    bool IS_IMPL_LINK;
    bool IS_AGG_LINK;
    bool IS_VISIBLE;
    bool IS_DEPLOYMENT_LINK;

    bool IS_COMPONENT_LINK;

    QPen pen;
    QPen arrowPen;
    QPen selectedPen;
    QPen selectedArrowPen;

    QBrush selectedBrush;
    QBrush brush;


    int circleRadius;
    int width;
    int height;

    QRect bRec;

    QLineF line;
    QLineF arrowHead;
    QGraphicsTextItem* label;

    bool inScene;
    bool instanceLink;

    // GraphMLItem interface
public slots:
    void aspectsChanged(QStringList aspects);
};



#endif // NODECONNECTION_H
