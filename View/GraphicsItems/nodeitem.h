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
#include <QMenu>
#include <QWidgetAction>
#include <QCheckBox>

#include "../../Model/node.h"
#include "../../Model/graphmldata.h"
#include "../table/attributetablemodel.h"
#include "graphmlitem.h"
#include "editabletextitem.h"

class EdgeItem;
class NodeView;

class NodeItem : public GraphMLItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum RESIZE_TYPE{NO_RESIZE, RESIZE, HORIZONTAL_RESIZE, VERTICAL_RESIZE};
    NodeItem(Node *node, NodeItem *parent, QStringList aspects, bool IN_SUBVIEW=false);
    ~NodeItem();


    void setGridVisible(bool visible);


    NodeItem* getParentNodeItem();

    QList<EdgeItem*> getEdgeItems();
    void setParentItem(QGraphicsItem* parent);
    QRectF boundingRect() const;
    QRectF minimumVisibleRect();
    QRectF expandedVisibleRect();
    QRectF currentItemRect();


    QPointF getClosestGridPoint(QPointF childCenterPoint);

    QRectF gridRect();

    QRectF getChildBoundingRect();
    QRectF getMinimumChildRect();

    void childPosUpdated();

    QPointF getGridPosition(int x, int y);


    bool isSelected();
    bool isLocked();
    void setLocked(bool locked);
    bool isPainted();
    bool isAncestorSelected();


    void addChildNodeItem(NodeItem* child);
    void removeChildNodeItem(NodeItem* child);
    bool intersectsRectangle(QRectF rect);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    bool hasVisibleChildren();

    bool labelPressed(QPointF mousePosition);
    bool iconPressed(QPointF mousePosition);
	bool lockPressed(QPointF mousePosition);


    NodeItem::RESIZE_TYPE resizeEntered(QPointF mousePosition);


    bool isExpanded();
    bool isHidden();
    void setHidden(bool hidden);


    void addEdgeItem(EdgeItem* line);
    void removeEdgeItem(EdgeItem* line);

    void setCenterPos(QPointF pos);
    QPointF centerPos();
    void adjustPos(QPointF delta);
    void adjustSize(QSizeF delta);

    void addChildOutline(NodeItem* nodeItem, QPointF gridPoint);
    void removeChildOutline(NodeItem* nodeItem);


    double getWidth();
    double getHeight();
    void resetSize();
    double getChildWidth();
    double getChildHeight();

    QPointF getNextChildPos(bool currentlySorting=false);

    Node* getNode();
    QString getNodeKind();
    QList<NodeItem*> getChildNodeItems();
    bool isPermanentlyCentered();

    qreal getGridSize();

    QStringList getAspects();

    void setGraphicsView(QGraphicsView* view);

    bool isInAspect();
    bool isSorted();
    void setSorted(bool isSorted);

	QMenu* getLockMenu();
    QRectF getLockIconSceneRect();

signals:
    void NodeItem_SortModel();
    void NodeItem_MoveSelection(QPointF delta);
    void NodeItem_ResizeSelection(QSizeF delta);
    void NodeItem_MoveFinished();
    void NodeItem_ResizeFinished();

 	void Nodeitem_HasFocus(bool hasFocus);

    void NodeItem_showLockMenu(NodeItem* nodeItem);
    void NodeItem_lockMenuClosed(NodeItem* nodeItem);
    //Node Edge Signals
    void setEdgesVisibility(bool visible);
    void setEdgesSelected(bool selected);

    void centerViewAspects();

    //DockNodeItem Signals
    void updateDockNodeItem();
    void updateOpacity(qreal opacity);

    void recentralizeAfterChange(GraphML* item);

    void nodeItemMoved();
    void itemMovedOutOfScene(NodeItem* item);


public slots:
    QPolygonF getResizePolygon();
    //QRectF
    void parentNodeItemMoved();
    //Model Signals
    void graphMLDataChanged(GraphMLData *data);
    void setOpacity(qreal opacity);
    void setSelected(bool selected);
    void setVisible(bool visible);

    void setPermanentlyCentralized(bool centralized);

    void aspectsChanged(QStringList aspects);

    void newSort();
    void modelSort();

    //void expandNode(bool expand);
    void setNodeExpanded(bool expanded);

    void updateModelPosition();
    void updateModelSize();

    void sceneRectChanged(QRectF sceneRect);
    void labelUpdated(QString newLabel);
    void setNewLabel(QString label = "");

    //Turn off visible gridlines;
    void toggleGridLines(bool on);

    void snapToGrid();
    void snapChildrenToGrid();

 	void menuClosed();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);



private:
    bool compareTo2Decimals(qreal num1, qreal num2);
    void updateModelData();


    void resizeToOptimumSize();
    void setWidth(qreal width);
    void setHeight(qreal height);
    void setSize(qreal w, qreal h);
    void setPos(qreal x, qreal y);
    void setPos(const QPointF &pos);
    void calculateGridlines();
    void setupAspect();
    void setupBrushes();
    void setupIcon();
    void setupLabel();
    void setupGraphMLConnections();
    void setupLockMenu();
    void setPaintObject(bool paint);
    void updateGraphMLPosition();
    void updateChildrenOnChange();
    void retrieveGraphMLData();
    void updateTextLabel(QString text=0);
    void updateParent();
    void updateParentModel();



    QStringList getChildrenKind();
    NodeItem* getChildNodeItemFromNode(Node* child);
    QPointF isOverGrid(const QPointF centerPosition);







    double getCornerRadius();
    double getChildCornerRadius();
    double getMaxLabelWidth();
    double getItemMargin() const;
    double getChildItemMargin();// const;

    QSizeF getModelSize();


    bool drawGridlines();







    QMenu* lockMenu;
    QWidgetAction* lockPos;
    QWidgetAction* lockSize;
    QWidgetAction* lockLabel;
    QWidgetAction* lockSortOrder;


    NodeItem* parentNodeItem;
    QStringList viewAspects;
    QStringList currentViewAspects;

    QGraphicsPixmapItem* icon;
    QGraphicsPixmapItem* lockIcon;

    QList<EdgeItem*> connections;


    QGraphicsView* parentView;

    QString nodeKind;
    QString minimumHeightStr;
    QString fileID;

    bool isNodeExpanded;
    bool isNodePressed;
    bool isNodeSelected;
    bool isGridVisible;
    bool isInSubView;
    bool isNodeCentralized;
    bool isNodeSorted;

    bool isNodeOnGrid;

    bool hasSelectionMoved;
    bool hasSelectionResized;

    bool GRIDLINES_ON;




    bool hidden;
    bool hasDefinition;

    bool LOCKED_POSITION;
    bool PAINT_OBJECT;






    NodeItem::RESIZE_TYPE currentResizeMode;

    QHash<QString, QRectF> outlineMap;

    //Current Width/Height
    double width;
    double height;

    double minimumHeight;
    double minimumWidth;

    double expandedHeight;
    double expandedWidth;

    QPointF previousScenePosition;




    QVector<QLineF> xGridLines;
    QVector<QLineF> yGridLines;


    QList<NodeItem*> childNodeItems;

    //Used to store the Color/Brush/Pen for the selected Style.
    QColor selectedColor;
    QColor color;

    QBrush selectedBrush;
    QBrush brush;
    QPen pen;
    QPen selectedPen;






    QRectF currentSceneRect;


    EditableTextItem* textItem;
    NodeView* view;
};

#endif // NODEITEM_H
