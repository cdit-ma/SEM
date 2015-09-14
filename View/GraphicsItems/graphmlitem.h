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
    GraphMLItem(GraphML* attachedGraph, GUI_KIND kind);
    void detach();
    bool isDeleting();
    ~GraphMLItem();
    QString getGraphMLDataValue(QString key);
    GraphML* getGraphML();
    AttributeTableModel* getAttributeTable();

    void setNodeView(NodeView* view);
    NodeView* getNodeView();

    bool isNodeItem();
    bool isEdgeItem();
    QString getID();

    virtual void setSelected(bool selected) = 0;
public slots:
    virtual void graphMLDataChanged(GraphMLData*) = 0;

signals:
    void GraphMLItem_TriggerAction(QString actionName);
    void GraphMLItem_SetCentered(GraphMLItem*);
    void GraphMLItem_CenterAspects();

    void GraphMLItem_SetGraphMLData(QString, QString, QString);
    void GraphMLItem_ConstructGraphMLData(GraphML*, QString);
    void GraphMLItem_DestructGraphMLData(GraphML*, QString);

    void GraphMLItem_AppendSelected(GraphMLItem*);
    void GraphMLItem_RemoveSelected(GraphMLItem*);
    void GraphMLItem_ClearSelection(bool updateTable = false);
    void GraphMLItem_PositionSizeChanged(GraphMLItem*, bool = false);
private:
    bool IS_DELETING;
    GraphML* attachedGraph;
    AttributeTableModel* table;

    NodeView* nodeView;
    GUI_KIND kind;
    QString ID;


    // QGraphicsItem interface
};

#endif // GRAPHMLITEM_H
