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
#include <QRadioButton>

#include "inputitem.h"

#include "../../Model/node.h"
#include "../../Model/graphmldata.h"
#include "../Table/attributetablemodel.h"
#include "graphmlitem.h"

class EdgeItem;
class NodeView;

class NodeItem : public GraphMLItem
{
    Q_OBJECT
    //Q_INTERFACES(QGraphicsItem)

public:
    enum MOUSEOVER_TYPE{MO_NONE, MO_ICON, MO_TOP_LABEL, MO_BOT_LABEL, MO_EXPANDLABEL, MO_DEFINITION, MO_HARDWAREMENU, MO_DEPLOYMENTWARNING, MO_TOPBAR, MO_CONNECT, MO_MODEL_CIRCLE, MO_MODEL_TR, MO_MODEL_BR, MO_MODEL_BL, MO_MODEL_TL, MO_EXPAND, MO_ITEM, MO_RESIZE, MO_RESIZE_HOR, MO_RESIZE_VER};
    enum ASPECT_POS{AP_NONE, AP_TOPLEFT, AP_TOPRIGHT,  AP_BOTRIGHT, AP_BOTLEFT};

    enum RESIZE_TYPE{NO_RESIZE, RESIZE, HORIZONTAL_RESIZE, VERTICAL_RESIZE};

    enum IMAGE_POS{IP_TOPLEFT, IP_TOPRIGHT, IP_BOT_RIGHT, IP_BOTLEFT, IP_CENTER};

    NodeItem(Node *node, NodeItem *parent, QStringList aspects, bool IN_SUBVIEW=false);
    ~NodeItem();



    MOUSEOVER_TYPE getMouseOverType(QPointF scenePos);

    void setEditableField(QString keyName, bool dropDown);
    //Used Methods
    void setZValue(qreal z);
    void restoreZValue();

    void setNodeConnectable(bool connectable);

    void childHidden();

    QColor getBackgroundColor();


    void setVisibleParentForEdgeItem(int ID, bool RIGHT = false);
    int getIndexOfEdgeItem(int ID, bool RIGHT = false);
    int getNumberOfEdgeItems(bool RIGHT = false);
    void removeVisibleParentForEdgeItem(int ID);

    void setGridVisible(bool visible);


    NodeItem* getParentNodeItem();

    QList<EdgeItem*> getEdgeItems();
    void setParentItem(QGraphicsItem* parent);
    QRectF boundingRect() const;
    QRectF childrenBoundingRect() const;
    QRectF minimumBoundingRect() const;
    QRectF expandedBoundingRect() const;
    QRectF expandedLabelRect() const;

    int getEdgeItemIndex(EdgeItem* item = 0);
    int getEdgeItemCount();

    QPointF getClosestGridPoint(QPointF childCenterPoint);

    QRectF gridRect();
    QPointF getCenterOffset();

    QRectF headerRect();
    QRectF bodyRect();


    QRectF getChildBoundingRect();



    QPointF getGridPosition(int x, int y);


    bool isHardwareHighlighted();
    void setHardwareHighlighting(bool highlighted);

    bool isLocked();
    void setLocked(bool locked);

    bool isModel();
    bool isAspect();
    bool isHardwareCluster();

    bool isAncestorSelected();


    void addChildNodeItem(NodeItem* child);
    void removeChildNodeItem(int ID);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void paintModel(QPainter* painter);
    void paintModel2(QPainter* painter);

    bool hasVisibleChildren();




    bool mouseOverModelCircle(QPointF mousePosition);
    bool mouseOverModelQuadrant(QPointF mousePosition);
    bool mouseOverModelTR(QPointF mousePosition);
    bool mouseOverModelBR(QPointF mousePosition);
    bool mouseOverModelBL(QPointF mousePosition);
    bool mouseOverModelTL(QPointF mousePosition);

    bool mouseOverRightLabel(QPointF mousePosition);
    bool mouseOverBotInput(QPointF mousePosition);
    bool mouseOverDeploymentIcon(QPointF mousePosition);
    bool mouseOverDefinition(QPointF mousePosition);
    bool mouseOverIcon(QPointF mousePosition);
    bool mouseOverTopBar(QPointF mousePosition);
    bool mouseOverHardwareMenu(QPointF mousePosition);
    bool mouseOverConnect(QPointF mousePosition);
    bool mouseOverExpand(QPointF mousePosition);



    NodeItem::RESIZE_TYPE resizeEntered(QPointF mousePosition);


    void addChildEdgeItem(EdgeItem* edge);
    QList<EdgeItem*> getChildEdges();
    void removeChildEdgeItem(EdgeItem* edge);
    bool isExpanded();
    bool isContracted();
    bool isHidden();
    void setHidden(bool hidden);


    void addEdgeItem(EdgeItem* line);
    void updateDefinition();
    void removeEdgeItem(EdgeItem* line);


    void setCenterPos(QPointF pos);
    QPointF centerPos();
    void adjustPos(QPointF delta);
    void adjustSize(QSizeF delta);

    void addChildOutline(NodeItem* nodeItem, QPointF gridPoint);
    void removeChildOutline(int ID);


    double getWidth();
    double getHeight();
    void resetSize();
    double getChildWidth();
    double getChildHeight();

    QPointF getNextChildPos(QRectF itemRect = QRectF(), bool currentlySorting=false);

    Node* getNode();
    QString getNodeKind();
    QString getNodeLabel();
    QList<NodeItem*> getChildNodeItems();


    qreal getGridSize();
    qreal getGridGapSize();

    QStringList getAspects();



    bool isInAspect();
    bool isSorted();
    void setSorted(bool isSorted);

    QMenu* getChildrenViewOptionMenu();
    QRectF geChildrenViewOptionMenuSceneRect();
    void showHardwareIcon(bool show);
    QList<NodeItem *> deploymentView(bool on, NodeItem* selectedItem = 0);

    int getChildrenViewMode();

    void dockHighlight(bool highlight);

signals:
    void NodeItem_Model_AspectToggled(int ID);
    //Node Edge Signals
    void setEdgeVisibility(bool visible);
    void setEdgeSelected(bool selected);


    void model_PositionChanged();

    void NodeItem_MoveSelection(QPointF delta);
    void NodeItem_ResizeSelection(int ID, QSizeF delta);
    void NodeItem_MoveFinished();
    void NodeItem_ResizeFinished(int ID);

 	void Nodeitem_HasFocus(bool hasFocus);

    void NodeItem_ShowHardwareMenu(NodeItem* nodeItem);
    void NodeItem_lockMenuClosed(NodeItem* nodeItem);




    //void centerViewAspects();

    //DockNodeItem Signals
    void updateDockNodeItem();
    void updateOpacity(qreal opacity);




    void NodeItem_Moved();

    void visibilityChanged(bool visible);
    void nodeItem_HardwareMenuClicked(int viewMode);


public slots:
    void labelEditModeRequest();
    void dataChanged(QString dataValue);
    void labelUpdated(QString newLabel);

    void childMoved();

    void zoomChanged(qreal zoom);
    //USED METHODS
    void graphMLDataChanged(GraphMLData *data);

    void setSelected(bool selected);
    void setVisibility(bool visible);


    void aspectsChanged(QStringList aspects);

    void sort();

    QPointF getAspectsLockedPoint(ASPECT_POS asPos);

    //void expandNode(bool expand);
    void setNodeExpanded(bool expanded);

    void updateModelPosition();
    void updateModelSize();

    void sceneRectChanged(QRectF sceneRect);

    void setNewLabel(QString label = "");

    //Turn off visible gridlines;
    void toggleGridLines(bool on);



    void snapToGrid();
    void snapChildrenToGrid();

 	void menuClosed();
    void updateChildrenViewMode(int viewMode = -1);
    void hardwareClusterMenuItemPressed();
    int getHardwareClusterChildrenViewMode();

    void themeChanged(int theme);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);



private:
    void updateTextVisibility();
    QRectF adjustRectForBorder(QRectF rect, qreal width);
    void updateDisplayedChildren(int viewMode);

//USED METHODS
    QRectF smallIconRect() const;

    QRectF iconRect() const;
    QRectF textRect_Top() const;
    QRectF textRect_Right() const;

    QRectF iconRect_TopLeft() const;
    QRectF iconRect_TopRight() const;
    QRectF iconRect_BottomLeft() const;
    QRectF iconRect_BottomRight() const;

    QRectF getImageRect(IMAGE_POS pos) const;
    void setImage(IMAGE_POS pos, QPixmap image);

    QString getIconURL();


    void paintPixmap(QPainter *painter, NodeItem::IMAGE_POS pos, QString alias, QString imageName);


    //USED PARAMETERS;
    bool hasIcon;
    bool showDeploymentWarningIcon;


    bool isInResizeMode();
    bool isMoveable();
    bool isResizeable();
    void updateModelData();


    void resizeToOptimumSize(MOUSEOVER_TYPE type = MO_RESIZE);
    void setWidth(qreal width);
    void setHeight(qreal height);
    void setSize(qreal w, qreal h);
    void setPos(qreal x, qreal y);
    void setPos(const QPointF &pos);
    void calculateGridlines();

    void setupAspect();
    void setupBrushes();
    void setupLabel();
    void setupGraphMLDataConnections();
    void updateGraphMLData();
    void setupChildrenViewOptionMenu();

    void updateGraphMLPosition();
    void retrieveGraphMLData();
    void updateTextLabel(QString text="");
    void childUpdated();

    void connectToGraphMLData(GraphMLData* data);
    void connectToGraphMLData(QString keyName);



    QStringList getChildrenKind();
    NodeItem* getChildNodeItemFromNode(Node* child);
    QPointF isOverGrid(const QPointF centerPosition);

    double getItemMargin() const;
    double getChildItemMargin();// const;

    double ignoreInsignificantFigures(double model, double current);

    QSizeF getModelSize();


    bool drawGridlines();


    QMenu* childrenViewOptionMenu;
    QRadioButton* allChildren;
    QRadioButton* connectedChildren;
    QRadioButton* unConnectedChildren;

    bool IS_HARDWARE_CLUSTER;
    bool IS_MODEL;
    bool IS_DEFINITION;

    int CHILDREN_VIEW_MODE;
    bool sortTriggerAction;
    bool eventFromMenu;

    NodeItem* parentNodeItem;
    QStringList viewAspects;


    QString nodeKind;
    QString nodeLabel;
    QString nodeType;
    QString fileID;

    bool isNodeExpanded;
    bool isGridVisible;
    bool isInSubView;
    bool isNodeSorted;
    bool isNodeInAspect;
    bool isNodeOnGrid;
    bool canNodeBeConnected;
    bool canNodeBeExpanded;
    bool hasHardwareWarning;



    bool hidden;
    bool hasChildren;

    bool GRIDLINES_ON;
    bool HAS_DEFINITION;
    bool IS_IMPL_OR_INST;

    bool LOCKED_POSITION;

    bool isNodeMoving;
    qreal oldZValue;
    NodeItem::RESIZE_TYPE currentResizeMode;

    QHash<int, QRectF> outlineMap;

    QHash<IMAGE_POS, QPixmap> imageMap;

    //Current Width/Height
    double width;
    double height;

    double minimumHeight;
    double minimumWidth;

    double expandedHeight;
    double expandedWidth;

    double modelHeight;
    double modelWidth;


    QPointF previousScenePosition;

    //USED TO DETERMINE THE NUMBER OF EDGES.
    QList<EdgeItem*> connections;

    QList<int> currentLeftEdgeIDs;
    QList<int> currentRightEdgeIDs;


    QVector<QLineF> xGridLines;
    QVector<QLineF> yGridLines;


    QHash<int, NodeItem*> childNodeItems;

    QList<int> childrenIDs;

    QList<EdgeItem*> childEdges;
    //QList<NodeItem*> childNodeItems;

    //Used to store the Color/Brush/Pen for the selected Style.
    QColor selectedColor;
    QColor color;

    QColor modelCircleColor;
    QColor topLeftColor;
    QColor topRightColor;
    QColor bottomLeftColor;
    QColor bottomRightColor;
    int darkThemeType;

    QBrush selectedBrush;
    QBrush bodyBrush;
    QBrush headerBrush;
    QPen pen;
    QPen selectedPen;
    bool updatedAlready;


    QPointF modelCenterPoint;

    bool hasPanned;


    qreal selectedPenWidth;

    MOUSEOVER_TYPE mouseDownType;


    QRectF currentSceneRect;


    ASPECT_POS aspectPos;
    QPointF aspectLockPos;

    //InputPut
    InputItem* bottomInputItem;
    InputItem* topLabelInputItem;
    InputItem* rightLabelInputItem;


    QString nodeHardwareOS;
    QString nodeHardwareArch;
    bool nodeHardwareLocalHost;
    bool nodeMemberIsKey;


    QList<int> connectedDataIDs;
    QString editableDataKey;
    bool hasEditData;
    bool editableDataDropDown;

    bool gotVisibleChildren;

    // GraphMLItem interface
public slots:
    bool canHover();
};

#endif // NODEITEM_H
