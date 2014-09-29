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
#include <QAbstractTableModel>

#include <QGraphicsItem>
#include "../Model/node.h"
#include "../Model/graphmldata.h"
#include <QRubberBand>
#include "attributetablemodel.h"

class NodeEdge;
class NodeItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

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
    AttributeTableModel* getTable();
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);


signals:
    void triggerSelected(NodeItem*);


    void deleted(NodeItem*);
    void centreNode(NodeItem*);
    void centreNode(Node*);
    void exportSelected(Node*);

    void setNodeSelected(Node*, bool selected=true);

    void actionTriggered(QString action);

    void makeChildNode(QPointF centerPoint, Node * parentNode);
    void makeChildNode(QPointF centerPoint);

    void moveSelection(QPointF move);
    void makeChildNode(QString type, Node*);
    void updateGraphMLData(Node*, QString, QString);
    void updateData(QString key, QString value);
public slots:
    void setOpacity(qreal opacity);
    void setSelected2();
    void setSelected(bool selected);
    void setDeselected2();
    void toggleDetailDepth(int level);
    void updatedData(GraphMLData* data);
    void recieveData();
    void destructNodeItem();
    void updateChildNodeType(QString type);
    void updateViewAspect(QString aspect);
    void sortChildren();

    void setRubberbandMode(bool On);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    void setParent(NodeItem* parentItem);

    QString viewAspect;

    AttributeTableModel* attributeModel;

    void updatePosition(QString x=0, QString y=0);
    void updateSize(QString w=0, QString h=0);
    QString toBuildType;
    QString name;
    QString kind;
    QRect bRec;


    bool USING_RUBBERBAND_SELECTION;

    QPointF origin;
    QPointF sceneOrigin;


    bool hasMoved;

    QVector<NodeEdge*> connections;

    QRubberBand* rubberBand;
    QGraphicsColorizeEffect *graphicsEffect;

    QGraphicsTextItem* label;
    bool isPressed;
    QPointF previousPosition;


};

#endif // NODEITEM_H
