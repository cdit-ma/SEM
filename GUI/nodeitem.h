#ifndef NODEITEM_H
#define NODEITEM_H

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsColorizeEffect>
#include <iostream>
#include <QTouchEvent>
#include <QObject>
#include <QAbstractItemModel>

#include <QGraphicsItem>
#include "../Model/node.h"
#include "../Model/graphmldata.h"
class NodeEdge;
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
    void addConnection(NodeEdge* line);
    void deleteConnnection(NodeEdge* line);



signals:
    void triggerSelected(NodeItem*);

    void deleted(NodeItem*);
    void centreNode(NodeItem*);
    void exportSelected(Node*);

    void actionTriggered(QString action);

    void makeChildNode(QString type, Node*);
    void updateGraphMLData(Node*, QString, QString);
    void updateData(QString key, QString value);
public slots:
    void setOpacity(qreal opacity);
    void setSelected();
    void setDeselected();
    void toggleDetailDepth(int level);
    void updatedData(GraphMLData* data);
    void recieveData();
    void destructNodeItem();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:



    void updatePosition(QString x=0, QString y=0);
    QString name;
    QString kind;
    QRect bRec;



    QVector<NodeEdge*> connections;


    QGraphicsColorizeEffect *graphicsEffect;

    QGraphicsTextItem* label;
    bool isPressed;
    QPointF previousPosition;

};

#endif // NODEITEM_H
