#ifndef NODECONNECTION_H
#define NODECONNECTION_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <iostream>
#include <QTouchEvent>
#include <QObject>
#include <QAbstractItemModel>

#include <QGraphicsItem>
#include "../Model/node.h"
#include "../Model/graphmldata.h"
#include "nodeitem.h"
#include <QObject>


class NodeConnection : public QObject, public QGraphicsLineItem
{
    Q_OBJECT

public:
    NodeConnection(Edge *edge, NodeItem* s, NodeItem* d);
    ~NodeConnection();

     void updateLine();
    void  addToScene(QGraphicsScene* scene);
signals:

public slots:
    void deleteD(Edge*);

private:
    QGraphicsLineItem* QGline;
    NodeItem* source;
    NodeItem* destination;

    QPen linePen;
    QLineF line;

    bool inScene;
    QPointF sPos;
    QPointF dPos;
    qreal sx;
    qreal sy;
    qreal dx;
    qreal dy;



};



#endif // NODECONNECTION_H
