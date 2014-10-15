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

#include <QGraphicsItem>
#include "../Model/node.h"
#include "../Model/graphmldata.h"
#include "nodeitem.h"
#include <QObject>


class NodeEdge: public GraphMLItem, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    NodeEdge(Edge *edge, NodeItem* s, NodeItem* d);
    ~NodeEdge();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    NodeItem* getSource();
    NodeItem* getDestination();
    void updateLine();
    Edge* edge;
    void  addToScene(QGraphicsScene* scene);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
signals:

public slots:
    void destructNodeEdge();
    void deleteD(Edge*);
    void setSelected(bool selected);

    void setOpacity(qreal opacity);

    void setVisible(bool visible);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);


private:
    QGraphicsLineItem* QGline;

    NodeItem* source;
    NodeItem* destination;


    int width;
    int height;

    QRect bRec;

    QPen linePen;
    QLineF line;
    QGraphicsTextItem* label;

    bool inScene;
    QPointF sPos;
    QPointF dPos;
    qreal sx;
    qreal sy;
    qreal dx;
    qreal dy;

    QGraphicsColorizeEffect *itemGraphicsEffect;
    QGraphicsColorizeEffect *lineGraphicsEffect;


};



#endif // NODECONNECTION_H
