#ifndef GRAPHMLITEM_H
#define GRAPHMLITEM_H
#include <QGraphicsObject>
#include <QPen>
#include "../../Model/graphml.h"
#include "../../Model/graphmldata.h"


class NodeView;
class AttributeTableModel;
class NodeItem;
class GraphMLItem: public QGraphicsObject
{
    Q_OBJECT

public:
    enum GUI_KIND{MODEL_ITEM, ASPECT_ITEM, ENTITY_ITEM, NODE_EDGE};
    enum ASPECT_POS{AP_NONE, AP_TOPLEFT, AP_TOPRIGHT,  AP_BOTRIGHT, AP_BOTLEFT};
    enum RENDER_STATE{RS_NONE, RS_MINIMAL, RS_REDUCED, RS_FULL};

    GraphMLItem(GraphML* graph, GraphMLItem *parent, GUI_KIND kind);
    ~GraphMLItem();
    virtual QRectF sceneBoundingRect() const;
    virtual QRectF boundingRect() const;
    QRectF translatedBoundingRect();
    QString getNodeKind();
    RENDER_STATE getRenderState() const;
    void setRenderState(RENDER_STATE renderState);

    virtual void lastChildRemoved();
    virtual void addChild(GraphMLItem* item);
    void removeChild(int ID);
    bool hasChildren();



    GraphMLItem* getParent();
    void setParent(GraphMLItem* item);
    QList<GraphMLItem*> getChildren();

    void connectToGraphMLData(QString keyName);
    void connectToGraphMLData(GraphMLData* data);
    void updateFromGraphMLData();
    void setGraphMLData(QString keyName, QString value);
    void setGraphMLData(QString keyName, qreal value);
    void detach();
    virtual void setInSubView(bool inSubview);
    bool isDeleting();

    bool intersectsRectangle(QRectF rect);


    QString getGraphMLDataValue(QString key);
    GraphML* getGraphML();
    int getID();

    AttributeTableModel* getAttributeTable();


    NodeView* getNodeView();


    bool inMainView();
    bool inSubView();
    bool isSelected();
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

    virtual void graphMLDataChanged(GraphMLData*) = 0;

    virtual void zoomChanged(qreal zoom);



signals:
    void GraphMLItem_TriggerAction(QString actionName);
    void GraphMLItem_SetCentered(GraphMLItem*);

    void GraphMLItem_SetGraphMLData(int, QString, QString);
    void GraphMLItem_SetGraphMLData(int, QString, qreal);
    void GraphMLItem_ConstructGraphMLData(GraphML*, QString);
    void GraphMLItem_DestructGraphMLData(GraphML*, QString);

    void GraphMLItem_AppendSelected(GraphMLItem*);
    void GraphMLItem_RemoveSelected(GraphMLItem*);
    void GraphMLItem_ClearSelection(bool updateTable = false);

    void GraphMLItem_PositionChanged();
    void GraphMLItem_SizeChanged();
    void GraphMLItem_Hovered(int ID, bool entered);
private:

    void setupPens();
    void updateCurrentPen(bool zoomChanged = false);
    bool IS_DELETING;
    bool IS_SELECTED;
    bool IS_HOVERED;
    bool IS_HIGHLIGHTED;
    bool IN_SUBVIEW;
    GraphML* attachedGraph;
    GraphMLItem* parentItem;
    AttributeTableModel* table;
    QList<int> connectedDataIDs;

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

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
};

#endif // GRAPHMLITEM_H
