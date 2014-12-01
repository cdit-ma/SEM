#ifndef NODECONNECTION_H
#define NODECONNECTION_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <iostream>
#include <QTouchEvent>
#include <QGraphicsColorizeEffect>
#include <QObject>
#include <QAbstractItemModel>


#include "../Model/node.h"
#include "../Model/edge.h"
#include "../Model/graphmldata.h"
#include "nodeitem.h"
#include <QObject>


class NodeEdge: public GraphMLItem
{
    Q_OBJECT

public:
    NodeEdge(Edge *edge, NodeItem* s, NodeItem* d);
    ~NodeEdge();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    NodeItem* getSource();
    NodeItem* getDestination();

    void updateLine();


    void  addToScene(QGraphicsScene* scene);

public slots:
    void destructNodeEdge();
    void setOpacity(qreal opacity);

    void setSelected(bool selected);
    void setVisible(bool visible);

    void graphMLDataUpdated(GraphMLData * data);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    void updateLabel();

    void updateBrushes();

    QGraphicsLineItem* QGline;
    QGraphicsLineItem* QGline2;
    NodeItem* source;
    NodeItem* destination;


    bool IS_SELECTED;
    bool IS_INSTANCE_LINK;
    bool IS_IMPL_LINK;
    bool IS_AGG_LINK;
    bool IS_VISIBLE;

    QPen pen;
    QPen arrowPen;
    QPen selectedPen;

    QColor selectedColor;
    QColor color;
    QBrush selectedBrush;
    QBrush brush;


    int width;
    int height;

    QRect bRec;

    QLineF line;
    QLineF arrowHead;
    QGraphicsTextItem* label;

    bool inScene;
    bool instanceLink;
};



#endif // NODECONNECTION_H
