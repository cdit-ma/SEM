#ifndef NODEITEMNEW_H
#define NODEITEMNEW_H

#include "entityitemnew.h"
#include "../nodeviewitem.h"
#include "edgeitemnew.h"
#include <QRectF>
#include <QMarginsF>
#include <QPainter>
#include <QPointF>
class NodeItemNew: public EntityItemNew
{
    Q_OBJECT
public:
    enum KIND{DEFAULT_ITEM, MODEL_ITEM, ASPECT_ITEM, PARAMETER_ITEM, QOS_ITEM, HARDWARE_ITEM, CONTAINER_ELEMENT_ITEM};
    NodeItemNew(NodeViewItem *viewItem, NodeItemNew* parentItem, KIND kind);
    ~NodeItemNew();

    NodeItemNew* getParentNodeItem() const;
    KIND getNodeItemKind();

    Node::NODE_KIND getNodeKind() const;


    virtual void setRightJustified(bool isRight);
    bool isRightJustified() const;

    virtual void addChildNode(NodeItemNew* nodeItem);

    void setIgnorePosition(bool ignore);
    bool isIgnoringPosition();

    void removeChildNode(NodeItemNew *nodeItem);

    int getSortOrder() const;

    bool hasChildNodes() const;
    QList<NodeItemNew*> getChildNodes() const;
    QList<NodeItemNew*> getOrderedChildNodes() const;
    QList<EntityItemNew*> getChildEntities() const;

    QPointF validateAdjustPos(QPointF delta);
    QPainterPath getChildNodePath();

    void addChildEdge(EdgeItemNew* edgeItem);
    void removeChildEdge(int ID);
    QList<EdgeItemNew*> getChildEdges();

    void addProxyEdge(EdgeItemNew* edgeItem);
    void removeProxyEdge(int ID);
    QList<EdgeItemNew*> getProxyEdges();

    QRectF getNearestGridOutline();
    QPointF getNearestGridPointToCenter();


    void setGridEnabled(bool enabled);
    bool isGridEnabled() const;
    void setGridVisible(bool visible);
    bool isGridVisible() const;

    void setResizeEnabled(bool enabled);
    bool isResizeEnabled();

    void setChildNodeMoving(NodeItemNew* child, bool moving);

    void setMoving(bool moving);

    virtual QRectF sceneBoundingRect() const;



    //RECTS
    virtual QRectF viewRect() const;
    virtual QRectF boundingRect() const;
    virtual QRectF contractedRect() const;
    virtual QRectF expandedRect() const;
    virtual QRectF currentRect() const;
    QRectF gridRect() const;
    QRectF expandedGridRect() const;
    virtual QRectF bodyRect() const;
    virtual QRectF headerRect() const;

    QRectF childrenRect() const;

    QSizeF getSize() const;

    void adjustExpandedSize(QSizeF delta);

    void setMinimumWidth(qreal width);
    void setMinimumHeight(qreal height);

    //Size/Position Functions
    void setExpandedWidth(qreal width, bool lockOnChange=false);
    void setExpandedHeight(qreal height, bool lockOnChange=false);
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


    virtual void setPos(const QPointF &pos);


    void setAspect(VIEW_ASPECT aspect);
    VIEW_ASPECT getAspect();

    void setManuallyAdjusted(RECT_VERTEX aspect);


    QMarginsF getMargin() const;
    QMarginsF getBodyPadding() const;


    void setPrimaryTextKey(QString key);
    void setSecondaryTextKey(QString key);
    QString getPrimaryTextKey() const;
    QString getSecondaryTextKey() const;
    bool gotPrimaryTextKey() const;
    bool gotSecondaryTextKey() const;


    QString getPrimaryText() const;
    QString getSecondaryText() const;

    virtual void setExpanded(bool expand);


    int getVertexAngle(RECT_VERTEX vert) const;
signals:
    void req_adjustSize(NodeItemNew* item, QSizeF delta, RECT_VERTEX vert);
    void req_adjustSizeFinished(NodeItemNew* item, RECT_VERTEX vert);

    //Inform of Changes
    void gotChildNodes(bool);
    void gotChildProxyEdges(bool);
public slots:
    virtual void dataChanged(QString keyName, QVariant data);
    virtual void dataRemoved(QString keyName);
    virtual void childPosChanged();
private:
    void resizeToChildren();
    int getResizeArrowRotation(RECT_VERTEX vert) const;


    void updateGridLines();
    NodeViewItem* nodeViewItem;
    KIND nodeItemKind;


    qreal minimumWidth;
    qreal minimumHeight;
    qreal expandedWidth;
    qreal expandedHeight;

    QMarginsF margin;
    QMarginsF bodyPadding;




    bool gridEnabled;
    bool gridVisible;
    bool ignorePosition;

    bool _rightJustified;

    bool horizontalLocked;
    bool verticalLocked;

    bool resizeEnabled;

    RECT_VERTEX hoveredResizeVertex;
    RECT_VERTEX selectedResizeVertex;

    QPointF previousMovePoint;
    QPointF previousResizePoint;
    VIEW_ASPECT aspect;

    QHash<int, NodeItemNew*> childNodes;
    QHash<int, EdgeItemNew*> childEdges;
    QHash<int, EdgeItemNew*> proxyChildEdges;

    QVector<NodeItemNew*> movingChildren;

    QVector<QLineF> gridLines_Minor_Horizontal;
    QVector<QLineF> gridLines_Major_Horizontal;
    QVector<QLineF> gridLines_Minor_Vertical;
    QVector<QLineF> gridLines_Major_Vertical;

    QString primaryTextKey;
    QString secondaryTextKey;

    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // EntityItemNew interface
public:
    virtual QRectF getElementRect(ELEMENT_RECT rect) const;
    virtual QPainterPath getElementPath(ELEMENT_RECT rect) const;
    virtual QRectF getResizeRect(RECT_VERTEX vert) const;
    virtual QRectF getResizeArrowRect(RECT_VERTEX vert) const;


    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);


};
#endif
