#ifndef GRAPHMLITEM_H
#define GRAPHMLITEM_H
#include <QObject>
#include <QGraphicsItem>
#include "../../Model/graphml.h"


class AttributeTableModel;
class GraphMLItem: public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum GUI_KIND{NODE_ITEM, NODE_EDGE};
    GraphMLItem(GraphML* attachedGraph, GUI_KIND kind);
    ~GraphMLItem();
    GraphML* getGraphML();
    AttributeTableModel* getAttributeTable();

    bool isNodeItem();
    bool isEdgeItem();

    virtual void setSelected(bool selected) = 0;
    virtual void setOpacity(qreal opacity) = 0;

public slots:
    virtual void graphMLDataChanged(GraphMLData*) = 0;
    virtual void aspectsChanged(QStringList aspects) = 0;

signals:
    void GraphMLItem_TriggerAction(QString actionName);
    void GraphMLItem_SetCentered(GraphMLItem*);
    void GraphMLItem_CenterAspects();

    void GraphMLItem_SetGraphMLData(GraphML*, QString, QString);
    void GraphMLItem_ConstructGraphMLData(GraphML*, QString);
    void GraphMLItem_DestructGraphMLData(GraphML*, QString);

    void GraphMLItem_AppendSelected(GraphMLItem*);
    void GraphMLItem_RemoveSelected(GraphMLItem*);
    void GraphMLItem_ClearSelection(bool updateTable);
    void GraphMLItem_MovedOutOfScene(GraphMLItem*);
private:
    GraphML* attachedGraph;
    AttributeTableModel* table;

    GUI_KIND kind;

    // QGraphicsItem interface
};

#endif // GRAPHMLITEM_H
