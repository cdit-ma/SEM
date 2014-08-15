#ifndef NODEITEM_H
#define NODEITEM_H

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

class NodeItem : public QObject, QGraphicsItem
{
    Q_OBJECT

public:
       NodeItem(Node *node, NodeItem *parent);
       QRectF boundingRect() const;
       void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
       Node* node;
       int width;
       int height;
signals:
       void setSelected(NodeItem*);
       void exportSelected(Node*);
protected:
       void mousePressEvent(QGraphicsSceneMouseEvent *event);
       void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
       void mouseMoveEvent(QGraphicsSceneMouseEvent *event);



private:
       QString name;
       QString kind;
       QRect bRec;


        bool isPressed;
        QPointF previousMousePosition;

};

#endif // NODEITEM_H
