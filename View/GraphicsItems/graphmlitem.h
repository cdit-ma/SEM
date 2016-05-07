#ifndef GRAPHMLITEM_H
#define GRAPHMLITEM_H
#include <QGraphicsObject>
#include <QPen>
#include "../../enumerations.h"
#include "../../Controller/entityadapter.h"

class NodeView;
class AttributeTableModel;
class NodeItem;
class GraphMLItem: public QGraphicsObject
{
    Q_OBJECT

public:
    enum GUI_KIND{MODEL_ITEM, ASPECT_ITEM, ENTITY_ITEM, NODE_EDGE};
    enum ASPECT_POS{AP_NONE, AP_TOPLEFT, AP_TOPRIGHT,  AP_BOTRIGHT, AP_BOTLEFT};
    enum RENDER_STATE{RS_NONE, RS_BLOCK, RS_MINIMAL, RS_REDUCED, RS_FULL};

    GraphMLItem(EntityAdapter *graph, GraphMLItem *parent, GUI_KIND kind);
    ~GraphMLItem();
    virtual QRectF sceneBoundingRect() const;
    virtual QRectF boundingRect() const;
    QRectF childrenBoundingRect();
    QRectF translatedBoundingRect();
    QString getNodeKind();
    RENDER_STATE getRenderStateFromZoom(qreal lod) const;
    RENDER_STATE getRenderState() const;
    VIEW_STATE getViewState() const;
    void setRenderState(RENDER_STATE renderState);


    virtual void firstChildAdded(GraphMLItem* child);
    virtual void lastChildRemoved();

    void addChild(GraphMLItem* item);
    void removeChild(int ID);
    bool hasChildren();



    GraphMLItem* getParent();
    void setParent(GraphMLItem* item);
    QList<GraphMLItem*> getChildren();

    void updateFromData();
    void setData(QString keyName, QVariant value);
    bool isDataDifferent(QString keyName, QVariant currentValue);
    void detach();
    virtual void setInSubView(bool inSubview);
    bool isDeleting();

    void updateData(QString keyName);
    bool intersectsRectangle(QRectF rect);

    void listenForData(QString keyName);


    QVariant getDataValue(QString key);
    bool hasGraphMLKey(QString key);
    EntityAdapter* getEntityAdapter();
    int getID();

    AttributeTableModel* getAttributeTable();


    NodeView* getNodeView();


    bool inMainView();
    bool inSubView();
    bool isSelected();
    bool isActiveSelected();
    bool isHovered();
    bool isHighlighted();
    bool isEntityItem();
    bool isAspectItem();
    bool isNodeItem();
    bool isEdgeItem();
    bool isModelItem();
    bool isDataEditable(QString keyName);

    QPen getCurrentPen();
    qreal getZoomFactor();

    void handleSelection(bool setSelected, bool controlDown);

    void handleHover(bool entered);

    QRectF adjustRectForPen(QRectF rect, QPen pen);

    void setNodeView(NodeView* view);
public slots:
    virtual bool canHover();
    virtual void setHovered(bool isHovered);
    virtual void setHighlighted(bool isHighlight);
    virtual void setSelected(bool selected);
    virtual void setActiveSelected(bool active);

    virtual void dataChanged(QString keyName, QVariant Data) = 0;
    virtual void dataRemoved(QString keyName);

    virtual void zoomChanged(qreal zoom);



signals:
    void GraphMLItem_TriggerAction(QString actionName);
    void GraphMLItem_SetCentered(GraphMLItem*);

    void GraphMLItem_SetData(int, QString, QVariant);
    void GraphMLItem_ConstructData(GraphML*, QString);
    void GraphMLItem_DestructData(GraphML*, QString);

    void GraphMLItem_AppendSelected(GraphMLItem*);
    void GraphMLItem_RemoveSelected(GraphMLItem*);
    void GraphMLItem_ClearSelection();
    void GraphMLItem_SetActiveSelected(GraphMLItem*);
    void GraphMLItem_SelectionChanged();

    void GraphMLItem_PositionChanged();
    void GraphMLItem_SizeChanged();
    void GraphMLItem_Hovered(int ID, bool entered);

private:

    void setupPens();
    void updateCurrentPen(bool zoomChanged = false);
    bool IS_DELETING;
    bool IS_SELECTED;
    bool IS_ACTIVE_SELECTED;
    bool IS_HOVERED;
    bool IS_HIGHLIGHTED;
    bool IN_SUBVIEW;
    EntityAdapter* attachedGraph;
    GraphMLItem* parentItem;
    AttributeTableModel* table;
    QList<QString> connectedDataKeys;

    QHash<int, GraphMLItem*> children;

    RENDER_STATE renderState;
    NodeView* nodeView;
    GUI_KIND kind;
    int ID;
    QString nodeKind;
    qreal currentZoomFactor;

    QPen defaultPen;
    QPen selectedPen;
    QPen currentPen;

    qreal selectedPenWidth;

    ERROR_TYPE errorType;



protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
};

#endif // GRAPHMLITEM_H
