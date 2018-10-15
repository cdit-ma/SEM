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
    NodeItem(NodeViewItem *viewItem, NodeItem* parentItem);
    ~NodeItem();

    NodeViewItem* getNodeViewItem() const;
    NODE_KIND getNodeKind() const;

    virtual QRectF getElementRect(EntityRect rect) const;


    virtual void setRightJustified(bool isRight);
    bool isRightJustified() const;

    virtual void addChildNode(NodeItem* nodeItem);

    void removeChildNode(NodeItem *nodeItem);

    int getSortOrder() const;
    int getSortOrderRow() const;
    int getSortOrderRowSubgroup() const;

    bool hasChildNodes() const;
    QSet<NodeItem*> getChildNodes();
    const QList<NodeItem*>& getSortedChildNodes();

    void RecalcSortedChildNodes();
    QList<EntityItem*> getChildEntities() const;

    void addChildEdge(EdgeItem* edgeItem);
    void removeChildEdge(EdgeItem* edgeItem);

    QRectF getNearestGridOutline();
    QPointF getNearestGridPointToCenter();


    bool setMoveFinished();



    QRectF boundingRect() const;


    //RECTS
    QRectF viewRect() const;
    QRectF currentRect() const;
    
    QRectF gridRect() const;

    QRectF bodyRect() const;
    QRectF headerRect() const;


    QRectF getEdgeConnectRect(EDGE_DIRECTION direction, EDGE_KIND kind) const;
    QRectF getEdgeConnectIconRect(EDGE_DIRECTION direction, EDGE_KIND kind) const;
    QRectF getEdgeDirectionRect(EDGE_DIRECTION direction) const;
    
    QRectF getNotificationRect() const;
    QRectF getNotificationRect(Notification::Severity severity) const;
    QRectF getExpandStateRect() const;


    QPointF getSceneEdgeTermination(EDGE_DIRECTION direction, EDGE_KIND kind) const;


    const QHash<EDGE_DIRECTION, QSet<EDGE_KIND> >& getAllVisualEdgeKinds() const;
    const QHash<EDGE_DIRECTION, QSet<EDGE_KIND> >& getVisualEdgeKinds() const;

    const QHash<EDGE_DIRECTION, QSet<EDGE_KIND> >& getCurrentVisualEdgeKinds() const;

    

    virtual bool isExpandEnabled();
    virtual QRectF childrenRect() const;



    void setContractedWidth(qreal width);
    void setContractedHeight(qreal height);

    
    qreal getContractedWidth() const;
    qreal getContractedHeight() const;
    QSizeF getContractedSize() const;

    void setMargin(QMarginsF margin);
    void setBodyPadding(QMarginsF bodyPadding);

    QPointF getMarginOffset() const;

    qreal getWidth() const;
    qreal getHeight() const;


    QPointF getCenterOffset() const;


    void setAspect(VIEW_ASPECT aspect);
    VIEW_ASPECT getAspect();

    QMarginsF getMargin() const;
    QMarginsF getBodyPadding() const;


    void setPrimaryTextKey(QString key);
    void setSecondaryTextKey(QString key);
    void setTertiaryTextKey(QString key);


    void addVisualEdgeKind(EDGE_DIRECTION direction, EDGE_KIND kind, bool update = true);
    void updateVisualEdgeKinds();
    void updateNotifications();
    void updateNestedNotifications();
    EDGE_KIND getVisualEdgeKind() const;

    

    QString getPrimaryTextKey() const;
    QString getSecondaryTextKey() const;
    QString getTertiaryTextKey() const;
    

    
    bool gotPrimaryTextKey() const;
    bool gotSecondaryTextKey() const;
    bool gotTertiaryTextKey() const;



    QString getPrimaryText() const;
    QString getSecondaryText() const;
    QString getTertiaryText() const;

    virtual void setExpanded(bool expand);


    virtual void childPosChanged(EntityItem* child);
signals:
    void childSizeChanged(EntityItem* child);
    void childPositionChanged(EntityItem* child);
    void childIndexChanged(EntityItem* child);
    void childCountChanged();

    void indexChanged();


    void req_connectEdgeMenu(QPointF scene_pos, EDGE_KIND kind, EDGE_DIRECTION direction);
    void req_connectEdgeMode(QPointF scene_pos, EDGE_KIND kind, EDGE_DIRECTION direction);
    void req_addNodeMenu(QPointF scene_pos, int index = -1);

public slots:
<<<<<<< HEAD
    virtual void dataChanged(const QString& key_name, const QVariant& data);
=======
    virtual void dataChanged(QString keyName, QVariant data);
    virtual void propertyChanged(QString propertyName, QVariant data);
    virtual void dataRemoved(QString keyName);
    virtual void childPosChanged();

>>>>>>> entity-chart
private:
    void edgeAdded(EDGE_DIRECTION direction, EDGE_KIND edgeKind, int ID);
    void edgeRemoved(EDGE_DIRECTION direction, EDGE_KIND edgeKind, int ID);

private:
    bool IsNotificationHovered(Notification::Severity severity) const;
    bool SetNotificationHovered(Notification::Severity severity, bool hovered);

    bool IsEdgeKnobHovered(const QPair<EDGE_DIRECTION, EDGE_KIND>& edge_knob) const;
    bool SetEdgeKnobHovered(const QPair<EDGE_DIRECTION, EDGE_KIND>& edge_knob, bool hovered);


    void expandContractHover(bool hovered, const QPointF& pos);
    void primaryTextHover(bool hovered, const QPointF& pos);
    void secondaryTextHover(bool hovered, const QPointF& pos);
    void tertiaryTextHover(bool hovered, const QPointF& pos);
    
    void notificationHover(bool hovered, const QPointF& pos);
    void edgeKnobHover(bool hovered, const QPointF& pos);
    void moveHover(bool hovered, const QPointF& pos);
    void lockHover(bool hovered, const QPointF& pos);
    
    
    QHash<EDGE_DIRECTION, QSet<EDGE_KIND> > my_visual_edge_kinds;
    QHash<EDGE_DIRECTION, QSet<EDGE_KIND> > all_visual_edge_kinds;
    
    QMultiMap<QPair<EDGE_DIRECTION, EDGE_KIND>, int> attached_edges;

    QSet< QPair<EDGE_DIRECTION, EDGE_KIND> > hovered_edge_kinds;

    QHash<Notification::Severity, int> notification_counts_;
    QSet<Notification::Severity> hovered_notifications_;
    
    VIEW_ASPECT aspect = VIEW_ASPECT::NONE;

    NodeViewItem* node_view_item = 0;

    qreal contracted_width = 0;
    qreal contracted_height = 0;

    qreal expanded_width = 0;
    qreal expanded_height = 0;


    int index_ = -1;
    int row_ = 0;
    int column_ = 0;

    QMarginsF margin;
    QMarginsF bodyPadding;


    QRectF _childRect;

    void clearEdgeKnobPressedState();
    void clearNotificationKnobPressedState();

    void SortedChildrenDirty();

    bool edge_knob_pressed = false;
    bool edge_knob_dragged = false;
    QRectF pressed_edge_knob_rect;
    EDGE_DIRECTION pressed_edge_knob_direction = EDGE_DIRECTION::SOURCE;
    EDGE_KIND pressed_edge_knob_kind = EDGE_KIND::NONE;

    bool notification_pressed = false;
    QRectF pressed_notification_rect;
    Notification::Severity pressed_notification = Notification::Severity::NONE;



    QString primaryTextKey;
    QString secondaryTextKey;
    QString tertiaryTextKey;

    bool right_justified = false;

    QPointF previousMovePoint;
    

    QSet<EdgeItem*> child_edges;
    
    QSet<NodeItem*> child_nodes;
    QList<NodeItem*> sorted_child_nodes;
    bool sorted_child_nodes_dirty = true;
    
    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
protected:
    void paintBackground(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // EntityItemNew interface
public:
    virtual QPainterPath getElementPath(EntityRect rect) const;
    QPointF getTopLeftOffset() const;
    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
};
#endif //NODEITEM_H
