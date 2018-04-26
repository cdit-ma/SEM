#ifndef NODEITEM_H
#define NODEITEM_H

#include "../entityitem.h"
#include "../../../../Controllers/ViewController/nodeviewitem.h"
#include "../Edge/edgeitem.h"
#include "../../../../../modelcontroller/nodekinds.h"
class NodeItem: public EntityItem
{
    Q_OBJECT
public:
    enum class RectVertex{NONE, LEFT, TOP_LEFT, TOP, TOP_RIGHT, RIGHT, BOTTOM_RIGHT, BOTTOM, BOTTOM_LEFT};
    
    static std::list<RectVertex> getRectVertex();
    enum KIND{DEFAULT_ITEM, MODEL_ITEM, ASPECT_ITEM, PARAMETER_ITEM, QOS_ITEM, HARDWARE_ITEM, CONTAINER_ITEM};
    enum NODE_READ_STATE{NORMAL, READ_ONLY_INSTANCE, READ_ONLY_DEFINITION};
    NodeItem(NodeViewItem *viewItem, NodeItem* parentItem, KIND kind);
    ~NodeItem();

    KIND getNodeItemKind();
    NodeViewItem* getNodeViewItem() const;

    NODE_KIND getNodeKind() const;

    NODE_READ_STATE getReadState() const;


    virtual void setRightJustified(bool isRight);
    bool isRightJustified() const;

    virtual void addChildNode(NodeItem* nodeItem);

    QColor getHeaderColor() const;


    void removeChildNode(NodeItem *nodeItem);

    int getSortOrder() const;
    int getSortOrderRow() const;
    int getSortOrderRowSubgroup() const;

    bool hasChildNodes() const;
    QList<NodeItem*> getChildNodes() const;
    QList<NodeItem*> getSortedChildNodes() const;
    QList<EntityItem*> getChildEntities() const;

    QPainterPath getChildNodePath();

    void addChildEdge(EdgeItem* edgeItem);
    void removeChildEdge(int ID);
    QList<EdgeItem*> getChildEdges() const;

    QRectF getNearestGridOutline();
    QPointF getNearestGridPointToCenter();


    void setGridEnabled(bool enabled);
    bool isGridEnabled() const;
    void setGridVisible(bool visible);
    bool isGridVisible() const;

    void setResizeEnabled(bool enabled);
    bool isResizeEnabled();

    void setChildMoving(EntityItem* child, bool moving);

    void setMoveStarted();
    bool setMoveFinished();

    void setResizeStarted();
    bool setResizeFinished();






    //RECTS
    virtual QRectF viewRect() const;
    virtual QRectF boundingRect() const;
    virtual QRectF contractedRect() const;
    virtual QRectF expandedRect() const;
    virtual QRectF currentRect() const;
    QRectF translatedHeaderRect() const;
    QRectF gridRect() const;
    QRectF expandedGridRect() const;
    virtual QRectF bodyRect() const;
    virtual QRectF headerRect() const;


    QRectF getEdgeConnectRect(EDGE_DIRECTION direction, EDGE_KIND kind) const;
    QRectF getEdgeConnectIconRect(EDGE_DIRECTION direction, EDGE_KIND kind) const;
    QRectF getEdgeDirectionRect(EDGE_DIRECTION direction) const;

    QSet< QPair<EDGE_DIRECTION, EDGE_KIND> > getEdgeConnectRectAtPos(QPointF pos) const;


    int getEdgeConnectPos(EDGE_DIRECTION direction, EDGE_KIND kind) const;

    QPointF getSceneEdgeTermination(EDGE_DIRECTION direction, EDGE_KIND kind) const;


    QMultiMap<EDGE_DIRECTION, EDGE_KIND> getAllVisualEdgeKinds() const;
    QMultiMap<EDGE_DIRECTION, EDGE_KIND> getVisualEdgeKinds() const;

    


    virtual QRectF childrenRect() const;

    QSizeF getSize() const;

    void adjustExpandedSize(QSizeF delta);

    void setMinimumWidth(qreal width);
    void setMinimumHeight(qreal height);

    //Size/Position Functions
    void setExpandedWidth(qreal width);
    void setExpandedHeight(qreal height);
    void setExpandedSize(QSizeF size);

    qreal getExpandedWidth() const;
    qreal getExpandedHeight() const;
    QSizeF getExpandedSize() const;
    qreal getMinimumWidth() const;
    qreal getMinimumHeight() const;

    void setMargin(QMarginsF margin);
    void setBodyPadding(QMarginsF bodyPadding);

    QPointF getMarginOffset() const;
    QPointF getBottomRightMarginOffset() const;
    QPointF getTopLeftSceneCoordinate() const;

    qreal getWidth() const;
    qreal getHeight() const;


    QPointF getCenterOffset() const;

    virtual QPointF getSceneEdgeTermination(bool left) const;


    //virtual void setPos(const QPointF &pos);


    void setAspect(VIEW_ASPECT aspect);
    VIEW_ASPECT getAspect();

    void setManuallyAdjusted(NodeItem::RectVertex aspect);


    QMarginsF getMargin() const;
    QMarginsF getBodyPadding() const;


    void setPrimaryTextKey(QString key);
    void setSecondaryTextKey(QString key);


    void addVisualEdgeKind(EDGE_DIRECTION direction, EDGE_KIND kind);
    EDGE_KIND getVisualEdgeKind() const;

    

    QString getPrimaryTextKey() const;
    QString getSecondaryTextKey() const;
    QPair<QString, QString> getSecondaryIconPath() const;
    void setSecondaryIconPath(QPair<QString, QString> pair);
    bool gotPrimaryTextKey() const;
    bool gotSecondaryTextKey() const;


    QString getPrimaryText() const;
    QString getSecondaryText() const;

    virtual void setExpanded(bool expand);


    QSizeF getGridAlignedSize(QSizeF size=QSizeF()) const;
    int getVertexAngle(NodeItem::RectVertex vert) const;
    virtual void childPosChanged(EntityItem* child);
signals:
    void childSizeChanged(EntityItem* child);
    void childPositionChanged(EntityItem* child);
    void childIndexChanged(EntityItem* child);
    void childCountChanged();

    void indexChanged();
    
    void req_connectEdgeMenu(QPointF scene_pos, EDGE_KIND kind, EDGE_DIRECTION direction);
    void req_connectEdgeMode(QPointF scene_pos, EDGE_KIND kind, EDGE_DIRECTION direction);

    void req_StartResize();
    void req_Resize(NodeItem* item, QSizeF delta, NodeItem::RectVertex vert);
    void req_FinishResize();

public slots:
    virtual void dataChanged(QString keyName, QVariant data);
    virtual void propertyChanged(QString propertyName, QVariant data);
    virtual void dataRemoved(QString keyName);
private:
    void edgeAdded(EDGE_DIRECTION direction, EDGE_KIND edgeKind, int ID);
    void edgeRemoved(EDGE_DIRECTION direction, EDGE_KIND edgeKind, int ID);


    void updateReadState();
    void setUpColors();
protected:
    void resizeToChildren();
private:
    int getResizeArrowRotation(NodeItem::RectVertex vert) const;


    void updateGridLines();
    NodeViewItem* nodeViewItem;
    KIND nodeItemKind;

    QMultiMap<EDGE_DIRECTION, EDGE_KIND> visual_edge_kinds;
    QSet< QPair<EDGE_DIRECTION, EDGE_KIND> > hovered_edge_kinds;
    QMultiMap<QPair<EDGE_DIRECTION, EDGE_KIND>, int> attached_edges;


    EDGE_KIND visualEdgeKind = EDGE_KIND::NONE;
    QString visualEntityIcon;

    NODE_READ_STATE readState;

    qreal minimumWidth;
    qreal minimumHeight;
    qreal expandedWidth;
    qreal expandedHeight;

    QMarginsF margin;
    QMarginsF bodyPadding;


    QRectF _childRect;
    bool gridEnabled;
    bool gridVisible;
    bool ignorePosition;

    void clearEdgeKnobPressedState();

    bool edge_knob_pressed = false;
    bool edge_knob_dragged = false;
    QRectF pressed_edge_knob_rect;
    EDGE_DIRECTION pressed_edge_knob_direction = EDGE_DIRECTION::SOURCE;
    EDGE_KIND pressed_edge_knob_kind = EDGE_KIND::NONE;



    bool _isResizing;
    QSizeF sizePreResize;


    bool _rightJustified;

    bool resizeEnabled;




    bool hoveredConnect;



    NodeItem::RectVertex hoveredResizeVertex;
    NodeItem::RectVertex selectedResizeVertex;

    QPointF previousMovePoint;
    QPointF previousResizePoint;
    VIEW_ASPECT aspect;

    QHash<int, NodeItem*> childNodes;
    QHash<int, EdgeItem*> childEdges;

    QPair<QString, QString> secondary_icon;

    QPainterPath gridLines;

    QColor readOnlyInstanceColor;
    QColor readOnlyDefinitionColor;
    QColor resizeColor;

    qreal modelWidth;
    qreal modelHeight;
    QPen gridLinePen;

    QString primaryTextKey;
    QString secondaryTextKey;

    QPair<QString, QString> tertiaryIcon;
    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // EntityItemNew interface
public:
    virtual QRectF getElementRect(EntityRect rect) const;
    virtual QPainterPath getElementPath(EntityRect rect) const;
    virtual QRectF getResizeRect(NodeItem::RectVertex vert) const;
    virtual QRectF getResizeArrowRect() const;


    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);



    // EntityItemNew interface

    // EntityItemNew interface
public:
    QPointF getTopLeftOffset() const;
private:
    QColor header_color;
};
#endif //NODEITEM_H
