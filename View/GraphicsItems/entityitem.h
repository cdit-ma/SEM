#ifndef ENTITYITEM_H
#define ENTITYITEM_H


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

#include "nodeitem.h"
#include "inputitem.h"
#include "statusitem.h"

#include "../../Model/node.h"
#include "../../Model/data.h"
#include "graphmlitem.h"

class EdgeItem;
class NodeView;

class EntityItem : public NodeItem
{
    Q_OBJECT
public:
    enum MOUSEOVER_TYPE{MO_NONE, MO_ICON, MO_TOP_LABEL, MO_BOT_LABEL_ICON, MO_BOT_LABEL, MO_EXPANDLABEL, MO_DEFINITION, MO_HARDWAREMENU, MO_DEPLOYMENTWARNING, MO_TOPBAR, MO_CONNECT, MO_MODEL_CIRCLE, MO_MODEL_TR, MO_MODEL_BR, MO_MODEL_BL, MO_MODEL_TL, MO_EXPAND, MO_ITEM, MO_RESIZE, MO_RESIZE_HOR, MO_RESIZE_VER};




    enum IMAGE_POS{IP_TOPLEFT, IP_TOPRIGHT, IP_BOT_RIGHT, IP_BOTLEFT, IP_CENTER};

    EntityItem(NodeAdapter* node, NodeItem *parent);
    ~EntityItem();



    MOUSEOVER_TYPE getMouseOverType(QPointF scenePos);

    void setEditableField(QString keyName, bool dropDown);
    void setStatusField(QString keyName, bool isInt);
    //Used Methods
    void setZValue(qreal z);
    void restoreZValue();

    void setNodeConnectable(bool connectable);


    void handleExpandState(bool newState);

    QColor getBackgroundColor();


    void setVisibleParentForEdgeItem(int ID, bool RIGHT = false);
    int getIndexOfEdgeItem(int ID, bool RIGHT = false);
    int getNumberOfEdgeItems(bool RIGHT = false);
    void removeVisibleParentForEdgeItem(int ID);

    void setGridVisible(bool visible);


    EntityItem* getParentEntityItem();

    QList<EdgeItem*> getEdgeItems();
    //void setParentItem(QGraphicsItem* parent);
    QRectF boundingRect() const;

    QRectF minimumRect() const;
    QRectF sceneBoundingRect() const;
    QRectF expandedBoundingRect() const;
    QRectF expandedLabelRect() const;

    int getEdgeItemIndex(EdgeItem* item = 0);
    int getEdgeItemCount();

    QRectF gridRect() const;

    QRectF headerRect();
    QRectF bodyRect();


    QRectF getChildBoundingRect();



    QPointF getGridPosition(int x, int y);


    bool isHardwareHighlighted();
    bool isNodeReadOnly();
    void setHardwareHighlighting(bool highlighted);


    bool isHardwareCluster();
    bool isHardwareNode();
    bool isVector();

    bool isAncestorSelected();

    QString getIconURL();
    QString getIconPrefix();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);



    bool mouseOverRightLabel(QPointF mousePosition);
    bool mouseOverBotInputIcon(QPointF mousePosition);
    bool mouseOverBotInput(QPointF mousePosition);
    bool mouseOverDeploymentIcon(QPointF mousePosition);
    bool mouseOverDefinition(QPointF mousePosition);
    bool mouseOverIcon(QPointF mousePosition);
    bool mouseOverTopBar(QPointF mousePosition);
    bool mouseOverHardwareMenu(QPointF mousePosition);
    bool mouseOverConnect(QPointF mousePosition);
    bool mouseOverExpand(QPointF mousePosition);



   // void addChildEdgeItem(EdgeItem* edge);
   // QList<EdgeItem*> getChildEdges();
    //void removeChildEdgeItem(EdgeItem* edge);
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

    void addChildOutline(EntityItem* EntityItem, QPointF gridPoint);
    void removeChildOutline(int ID);

    qreal getWidth() const;
    qreal getHeight() const;

    qreal getCurrentWidth() const;
    qreal getCurrentHeight() const;

    double getExpandedWidth() const;
    double getExpandedHeight() const;


    void resetSize();
    double getChildWidth();
    double getChildHeight();




    NodeAdapter* getNodeAdapter();
    QString getNodeLabel();
    QList<EntityItem*> getChildEntityItems();


    qreal getGridSize() const;
    qreal getGridGapSize() const;



    QMenu* getChildrenViewOptionMenu();
    QRectF geChildrenViewOptionMenuSceneRect();
    void showHardwareIcon(bool show);
    QList<EntityItem *> deploymentView(bool on, EntityItem* selectedItem = 0);

    int getChildrenViewMode();

    void dockHighlight(bool highlight);

    void forceExpandParentItem();


signals:
    void EntityItem_Model_AspectToggled(int ID);
    //Node Edge Signals
    void setEdgeVisibility(bool visible);
    void setEdgeSelected(bool selected);


    void model_PositionChanged();

    void EntityItem_MoveSelection(QPointF delta);

    void EntityItem_MoveFinished();

    void EntityItem_HasFocus(bool hasFocus);

    void EntityItem_ShowHardwareMenu(EntityItem* EntityItem);
    void EntityItem_lockMenuClosed(EntityItem* EntityItem);




    //void centerViewAspects();

    //DockEntityItem Signals
    void updateOpacity(qreal opacity);




    void EntityItem_Moved();

    void visibilityChanged(bool visible);
    void EntityItem_HardwareMenuClicked(int viewMode);


    void entityItem_iconChanged();

    void entityItem_firstChildAdded(int parentID);
    void entityItem_lastChildRemoved(int parentID);


public slots:
    void labelEditModeRequest();
    void dataChanged(QString dataValue);
    void labelUpdated(QString newLabel);


    void zoomChanged(qreal zoom);
    RENDER_STATE getRenderStateFromZoom(qreal zoom);


    //USED METHODS
    void dataChanged(QString keyName, QVariant data);

    void setSelected(bool selected);
    void setVisibility(bool visible);





    //void expandNode(bool expand);


    void sceneRectChanged(QRectF sceneRect);

    void setNewLabel(QString label = "");

    //Turn off visible gridlines;
    void toggleGridLines(bool on);



    //void snapToGrid();


    void menuClosed();
    void updateChildrenViewMode(int viewMode = -1);
    void hardwareClusterMenuItemPressed();
    int getHardwareClusterChildrenViewMode();

    void themeChanged(VIEW_THEME theme);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    //void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);



private:
    void updateTextVisibility();

    void updateDisplayedChildren(int viewMode);

//USED METHODS
    QRectF smallIconRect() const;

    QRectF iconRect() const;
    QRectF textRect_Top() const;
    QRectF textRect_Right() const;
    QRectF textRect_Bot() const;

    QRectF iconRect_TopLeft() const;
    QRectF iconRect_TopRight() const;
    QRectF iconRect_BottomLeft() const;
    QRectF iconRect_BottomRight() const;

    QRectF getImageRect(IMAGE_POS pos) const;
    void setImage(IMAGE_POS pos, QPixmap image);


    void paintPixmap(QPainter *painter, EntityItem::IMAGE_POS pos, QString alias, QString imageName, bool update=false);


    //USED PARAMETERS;
    bool showDeploymentWarningIcon;


    bool isInResizeMode();
    bool isMoveable();
    bool isResizeable();


    void setExpandedWidth(qreal width);
    void setExpandedHeight(qreal height);


    bool isExpandedState();

    void setSize(qreal w, qreal h);
    void setPos(qreal x, qreal y);
    void setPos(const QPointF pos);

    void calculateGridlines();


    void setupBrushes();
    void setupLabel();
    void setupDataConnections();
    void setupChildrenViewOptionMenu();

    void retrieveData();
    void updateTextLabel(QString text="");
    void childUpdated();




    QStringList getChildrenKind();
    QPointF isOverGrid(const QPointF centerPosition);

    qreal getItemMargin() const;
    double getChildItemMargin();// const;

    double ignoreInsignificantFigures(double model, double current);

    QSizeF getModelSize();





    QMenu* childrenViewOptionMenu;
    QRadioButton* allChildren;
    QRadioButton* connectedChildren;
    QRadioButton* unConnectedChildren;

    bool IS_HARDWARE_CLUSTER;
    bool IS_HARDWARE_NODE;
    bool IS_VECTOR;

    int CHILDREN_VIEW_MODE;
    bool sortTriggerAction;
    bool eventFromMenu;

    EntityItem* parentEntityItem;




    QString nodeKind;
    QString nodeLabel;
    QString nodeType;
    QString workerKind;
    QString operationKind;
    QString fileID;

    bool isGridVisible;
    bool isNodeSorted;
    bool isNodeOnGrid;
    bool canNodeBeConnected;

    bool hasHardwareWarning;



    bool hidden;

    bool GRIDLINES_ON;
    bool HAS_DEFINITION;
    bool IS_IMPL_OR_INST;


    bool IS_EXPANDED_STATE;

    bool isNodeMoving;
    qreal oldZValue;
    RESIZE_TYPE currentResizeMode;

    QHash<int, QRectF> outlineMap;

    QHash<IMAGE_POS, QPixmap> imageMap;

    double contractedHeight;
    double contractedWidth;

    double expandedHeight;
    double expandedWidth;



    QPointF previousScenePosition;

    //USED TO DETERMINE THE NUMBER OF EDGES.
    QList<EdgeItem*> connections;

    QList<int> currentLeftEdgeIDs;
    QList<int> currentRightEdgeIDs;


    QVector<QLineF> xGridLines;
    QVector<QLineF> yGridLines;

    //QList<EdgeItem*> childEdges;
    //QList<EntityItem*> childEntityItems;

    //Used to store the Color/Brush/Pen for the selected Style.
    QColor selectedColor;
    QColor color;

    QColor modelCircleColor;
    QColor topLeftColor;
    QColor topRightColor;
    QColor bottomLeftColor;
    QColor bottomRightColor;

    QBrush selectedBrush;
    QBrush bodyBrush;
    QBrush headerBrush;

    QBrush readOnlyBodyBrush;
    QBrush readOnlyHeaderBrush;

    QPen pen;
    QPen selectedPen;
    bool updatedAlready;


    QPointF modelCenterPoint;

    bool hasPanned;


    qreal selectedPenWidth;

    MOUSEOVER_TYPE mouseDownType;


    QRectF currentSceneRect;




    //InputPut
    InputItem* bottomInputItem;
    InputItem* topLabelInputItem;
    InputItem* rightLabelInputItem;

    StatusItem* statusItem;

    QString descriptionValue;
    QString nodeHardwareOS;
    QString nodeHardwareArch;
    bool nodeHardwareLocalHost;
    bool nodeMemberIsKey;

    bool IS_READ_ONLY;

    bool isInputParameter;
    bool isReturnParameter;

    QString vectorIconURL;
    bool changeIcon;

    QList<int> connectedDataIDs;
    QString editableDataKey;
    QString statusModeDataKey;
    bool hasEditData;
    bool editableDataDropDown;
    // GraphMLItem interface
public slots:
    bool canHover();

public:
    qreal getChildSize();
    qreal getChildMargin();

    // NodeItem interface


    // NodeItem interface
public slots:
    void childPositionChanged();
    void childSizeChanged();

    // GraphMLItem interface
public:
    void firstChildAdded(GraphMLItem* child);
    void lastChildRemoved();

    // NodeItem interface
public:
    void setWidth(qreal w);
    void setHeight(qreal h);
};

#endif // ENTITYITEM_H
