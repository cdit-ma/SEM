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
       ~NodeItem();
       QRectF boundingRect() const;
       void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
       Node* node;
       int width;
       int height;
signals:
       void setSelected(NodeItem*);
       void exportSelected(Node*);
       void updateData(QString key, QString value);
public slots:
       void recieveData();
       void deleteD(GraphMLContainer*);
protected:
       void mousePressEvent(QGraphicsSceneMouseEvent *event);
       void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
       void mouseMoveEvent(QGraphicsSceneMouseEvent *event);



private:
       QString name;
       QString kind;
       QRect bRec;

        QGraphicsTextItem* label;


        bool isPressed;
        QPointF previousMousePosition;

};

#endif // NODEITEM_H
