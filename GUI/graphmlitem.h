#ifndef GRAPHMLITEM_H
#define GRAPHMLITEM_H
#include <QObject>
#include <QGraphicsItem>
#include "../Model/graphml.h"


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
    bool isNodeEdge();

    virtual void setSelected(bool selected) = 0;
    virtual void setOpacity(qreal opacity) = 0;

public slots:
    virtual void graphMLDataUpdated(GraphMLData*) = 0;
signals:
    void triggerSelected(GraphML*, bool setSelected=true);
    void triggerCentered(GraphML* element);
    void triggerAction(QString actionName);

    void constructGraphMLData(GraphML* element, QString keyName);
    void destructGraphMLData(GraphML* element, QString keyName);
    void updateGraphMLData(GraphML* element, QString keyName, QString value);

private:
    GraphML* attachedGraph;
    AttributeTableModel* table;

    GUI_KIND kind;

    // QGraphicsItem interface
};

#endif // GRAPHMLITEM_H
