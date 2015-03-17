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
#include "editabletextitem.h"

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
    QRectF minimumVisibleRect() const;

    QPointF getGridPosition(int x, int y);
    bool isSelected();
    bool isPositionLocked();
    bool isPainted();
    bool isAncestorSelected();
    void setDrawGrid(bool value);

    void addChildNodeItem(NodeItem* child);
    void removeChildNodeItem(NodeItem* child);
    bool intersectsRectangle(QRectF rect);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    bool hasChildren();
    bool labelPressed(QPointF mousePosition);
    bool iconPressed(QPointF mousePosition);

    bool isExpanded();
    bool isHidden();
    void setHidden(bool hidden);

    void addNodeEdge(NodeEdge* line);
    void removeNodeEdge(NodeEdge* line);
    void adjustPos(QPointF delta);


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

    qreal getGridSize();

signals:
    void NodeItem_SortModel();
    void NodeItem_MoveSelection(QPointF delta);
    void NodeItem_MoveFinished();

    //Node Edge Signals
    void setEdgeVisibility(bool visible);
    void setEdgeSelected(bool selected);

    void centerViewAspects();

    //DockNodeItem Signals
    void updateDockNodeItem();
    void updateOpacity(qreal opacity);

    void addExpandButtonToParent();
    void updateParentHeight(NodeItem* child);

    void recentralizeAfterChange(GraphML* item);

    void nodeItemMoved();
    //void itemMovedOutOfScene(NodeItem* item);


public slots:
    void parentNodeItemMoved();
    //Model Signals
    void graphMLDataChanged(GraphMLData *data);
    void setOpacity(qreal opacity);
    void setSelected(bool selected);
    void setVisible(bool visible);

    void setPermanentlyCentralized(bool centralized);

    void aspectsChanged(QStringList aspects);
    void sort();

    void addExpandButton();
    void expandItem(bool show);

    void updateHeight(NodeItem* child);

    void updateModelPosition();

    void sceneRectChanged(QRectF sceneRect);
    void setNewLabel(QString label = "");

    //Turn off visible gridlines;
    void toggleGridLines(bool on);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);


private:
    NodeItem* getChildNodeItemFromNode(Node* child);
    void setWidth(qreal width);
    void setHeight(qreal height);

    void updateGridLines(bool updateX = false, bool updateY = false);

    void setPermanentlyInvisible(bool isInvisible);
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

    QColor invertColor(QColor oldColor);

    double getItemMargin() const;



    NodeItem* parentNodeItem;
    QStringList viewAspects;

    void setPos(qreal x, qreal y);
    void setPos(const QPointF &pos);

    QGraphicsPixmapItem* icon;
    QGraphicsPixmapItem* lockIcon;
    QGraphicsProxyWidget *proxyWidget;
    QGraphicsProxyWidget *labelWidget;
    QPushButton *expandButton;
    QPushButton* labelButton;

    QList<NodeEdge*> connections;

    bool nodeSelected;
    bool isNodePressed;
    bool drawGrid;

    int X_GRID_POS;
    int Y_GRID_POS;

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


    QVector<QLineF> xGridLines;
    QVector<QLineF> yGridLines;
    bool GRIDLINES_VISIBLE;

    QList<NodeItem*> childNodeItems;

    //Used to store the Color/Brush/Pen for the selected Style.
    QColor selectedColor;
    QColor color;
    QBrush selectedBrush;
    QBrush brush;
    QPen pen;
    QPen selectedPen;

    int nextX;
    int nextY;

    bool hasDefinition;
    bool onGrid;

    bool LOCKED_POSITION;

    bool PAINT_OBJECT;

    bool hidden;
    int depth;

    bool permanentlyInvisible;

    QRectF currentSceneRect;

    QPixmap pixmap;
    QString stringLabel;

    EditableTextItem* textItem;

    // GraphMLItem interface

};

#endif // NODEITEM_H
