#ifndef GRAPHMLITEM_H
#define GRAPHMLITEM_H
#include <QObject>
#include <QGraphicsItem>
#include "../../Model/graphml.h"
#include "../../Model/graphmldata.h"


class NodeView;
class AttributeTableModel;

class GraphMLItem: public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum GUI_KIND{NODE_ITEM, NODE_EDGE};
    enum RENDER_STATE{RS_NONE, RS_MINIMAL, RS_REDUCED, RS_FULL};

    GraphMLItem(GraphML* attachedGraph, GUI_KIND kind);
    ~GraphMLItem();

    RENDER_STATE getRenderState() const;
    void setRenderState(RENDER_STATE renderState);

    void detach();
    bool isDeleting();

    bool intersectsRectangle(QRectF rect);


    QString getGraphMLDataValue(QString key);
    GraphML* getGraphML();
    int getID();

    AttributeTableModel* getAttributeTable();

    void setNodeView(NodeView* view);
    NodeView* getNodeView();



    bool isSelected();
    bool isHighlighted();
    bool isNodeItem();
    bool isEdgeItem();
    bool isDataEditable(QString keyName);


    qreal getZoomFactor();

    void handleSelection(bool setSelected, bool controlDown);
    void handleHighlight(bool entered);
public slots:
    virtual bool canHighlight();
    virtual void setHighlighted(bool isHighlighted);

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
    void GraphMLItem_PositionSizeChanged(GraphMLItem*, bool = false);

    void GraphMLItem_Hovered(int ID, bool entered);
private:
    bool IS_DELETING;
    bool IS_SELECTED;
    bool IS_HIGHLIGHTED;
    GraphML* attachedGraph;
    AttributeTableModel* table;

    RENDER_STATE renderState;
    NodeView* nodeView;
    GUI_KIND kind;
    int ID;
    qreal currentZoomFactor;


    // QGraphicsItem interface
};

#endif // GRAPHMLITEM_H
