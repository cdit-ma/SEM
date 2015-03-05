#ifndef NODEITEM_H
#define NODEITEM_H

#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsColorizeEffect>
#include <QGraphicsSceneMouseEvent>
#include <QObject>
#include <QPainter>
#include <QTouchEvent>
#include <iostream>
#include <QGraphicsProxyWidget>
#include <QPushButton>

#include "../Model/node.h"
#include "../Model/graphmldata.h"
#include "attributetablemodel.h"
#include "graphmlitem.h"

class NodeEdge;

class NodeItem : public GraphMLItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    NodeItem(Node *node, NodeItem *parent, QStringList aspects, bool IN_SUBVIEW=false);
    ~NodeItem();

    NodeItem* getParentNodeItem();

    QList<NodeEdge*> getEdgeItems();
    void setParentItem(QGraphicsItem* parent);
    QRectF boundingRect() const;
    bool isSelected();
    bool isPainted();


    void addChildNodeItem(NodeItem* child);
    void removeChildNodeItem(NodeItem* child);
    bool intersectsRectangle(QRectF rect);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    bool hasChildren();

    bool isExpanded();
    bool isHidden();
    void setHidden(bool hidden);

    void addNodeEdge(NodeEdge* line);
    void removeNodeEdge(NodeEdge* line);

    double getWidth();
    double getHeight();
    void resetSize();
    double getChildWidth();

    QPointF getNextChildPos();
    void resetNextChildPos();

    Node* getNode();
    QString getNodeKind();
    QList<NodeItem*> getChildNodeItems();
    bool isPermanentlyCentered();


signals:
    //Node Edge Signals
    void setEdgeVisibility(bool visible);
    void setEdgeSelected(bool selected);

    //View Signals.
    void moveSelection(QPointF delta);
    void growSelection(QPointF delta);
    void clearSelection();

    void centerViewAspects();
    void sortModel();


    //DockNodeItem Signals
    void updateDockNodeItem();
    void updateOpacity(qreal opacity);

    void addExpandButtonToParent();
    void updateParentHeight(NodeItem* child);

    void recentralizeAfterChange(GraphML* item);

    void nodeItemMoved();
    void itemMovedOutOfScene(NodeItem* item);


public slots:
    void parentNodeItemMoved();
    //Model Signals
    void graphMLDataUpdated(GraphMLData *data);
    void setOpacity(qreal opacity);
    void setSelected(bool selected);
    void setVisible(bool visible);

    void setPermanentlyCentralized(bool centralized);

    void sort();

    //Depth/Aspect Slots
    void updateViewAspects(QStringList aspects);

    void addExpandButton();
    void expandItem(bool show);

    void updateHeight(NodeItem* child);

    void updateSceneRect(QRectF sceneRect);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    NodeItem* getChildNodeItemFromNode(Node* child);
    void setWidth(qreal width);
    void setHeight(qreal height);

    void setupAspect();
    void setupBrushes();
    void setupIcon();
    void setupLabel();
    void setupGraphMLConnections();

    void setPaintObject(bool paint);

    void updateGraphMLSize();
    void updateGraphMLPosition();

    void updateChildrenOnChange();
    void retrieveGraphMLData();

    void updateTextLabel(QString text=0);
    void updateExpandButton();

    bool hasExpandButton();
    void removeExpandButton();

    bool expanded;
    bool permanentlyCentralized;

    QStringList getChildrenKind();

    double getCornerRadius();
    double getMaxLabelWidth();


    NodeItem* parentNodeItem;
    QStringList viewAspects;

    void setPos(qreal x, qreal y);
    void setPos(const QPointF &pos);


    //Visual Components
    QGraphicsTextItem* label;
    QGraphicsPixmapItem* icon;
    QGraphicsPixmapItem* lockIcon;
    QGraphicsProxyWidget *proxyWidget;
    QPushButton *expandButton;

    QList<NodeEdge*> connections;

    bool nodeSelected;
    bool isNodePressed;

    QString nodeKind;
    QString fileID;

    double width;
    double height;

    double initialWidth;
    double initialHeight;

    double minimumHeight;
    double minimumWidth;

    double prevWidth;
    double prevHeight;



    QPointF nextChildPosition;

    QPointF previousScenePosition;
    //QPointF initialScenePressPosition;
    bool hasSelectionMoved;

    QList<NodeItem*> childNodeItems;

    //Used to store the Color/Brush/Pen for the selected Style.
    QColor selectedColor;
    QColor color;
    QBrush selectedBrush;
    QBrush brush;
    QPen pen;
    QPen selectedPen;

    bool hasDefinition;

    bool PAINT_OBJECT;

    bool hidden;
    int depth;

    QRectF currentSceneRect;

};

#endif // NODEITEM_H
