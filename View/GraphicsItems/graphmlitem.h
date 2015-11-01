#ifndef GRAPHMLITEM_H
#define GRAPHMLITEM_H
#include <QGraphicsObject>
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

    GraphMLItem(GraphML* attachedGraph, GUI_KIND kind);
    ~GraphMLItem();

    QString getNodeKind();
    RENDER_STATE getRenderState() const;
    void setRenderState(RENDER_STATE renderState);

    void addChild(GraphMLItem* item);
    void removeChild(int ID);
    bool hasChildren();
    GraphMLItem* getParent();
    void setParent(GraphMLItem* item);
    QList<GraphMLItem*> getChildren();

    void connectToGraphMLData(QString keyName);
    void connectToGraphMLData(GraphMLData* data);
    void updateFromGraphMLData();
    void setGraphMLData(QString keyName, QString value);
    void detach();
    void setInSubView(bool inSubview);
    bool isDeleting();

    bool intersectsRectangle(QRectF rect);


    QString getGraphMLDataValue(QString key);
    GraphML* getGraphML();
    int getID();

    AttributeTableModel* getAttributeTable();

    void setNodeView(NodeView* view);
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


    qreal getZoomFactor();

    void handleSelection(bool setSelected, bool controlDown);
    void handleHighlight(bool entered);

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
    void GraphMLItem_CenterAspects();

    void GraphMLItem_SetGraphMLData(int, QString, QString);
    void GraphMLItem_ConstructGraphMLData(GraphML*, QString);
    void GraphMLItem_DestructGraphMLData(GraphML*, QString);

    void GraphMLItem_AppendSelected(GraphMLItem*);
    void GraphMLItem_RemoveSelected(GraphMLItem*);
    void GraphMLItem_ClearSelection(bool updateTable = false);

    void GraphMLItem_PositionChanged();
    void GraphMLItem_SizeChanged();
    void GraphMLItem_Hovered(int ID, bool entered);
private:
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


    // QGraphicsItem interface
};

#endif // GRAPHMLITEM_H
