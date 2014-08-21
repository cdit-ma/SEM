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
#include "../Model/graphmldata.h"
class NodeConnection;
class NodeItem : public QObject, public QGraphicsItem
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
    bool drawDetail;
    bool drawObject;
    int depth;
    void notifyEdges();
    void addConnection(NodeConnection* line);
    void deleteConnnection(NodeConnection* line);
signals:
    void setSelected(NodeItem*);
    void exportSelected(Node*);

    void makeChildNode(Node*);
    void updateData(QString key, QString value);
public slots:

    void toggleDetailDepth(int level);
    void updatedData(GraphMLData* data);
    void recieveData();
    void deleteD(GraphMLContainer*);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:



    void updatePosition(QString x=0, QString y=0);
    QString name;
    QString kind;
    QRect bRec;



    QVector<NodeConnection*> connections;

    QGraphicsTextItem* label;
    bool isPressed;
    QPointF previousPosition;

};

#endif // NODEITEM_H
