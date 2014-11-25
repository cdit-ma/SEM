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
    GraphMLItem(GraphML* attachedGraph);
    ~GraphMLItem();
    GraphML* getGraphML();
    AttributeTableModel* getAttributeTable();

    virtual void setSelected(bool selected) = 0;
    virtual void setOpacity(qreal opacity) = 0;
signals:
    void setItemSelected(GraphML*, bool setSelected=true);
    void actionTriggered(QString actionName);
    void updateGraphMLData(GraphML* element, QString keyName, QString value);

    void center(GraphML* element);
    void destructGraphMLData(GraphML* element, QString keyName);
    void constructGraphMLData(GraphML* element, QString keyName);
private:
    GraphML* attachedGraph;
    AttributeTableModel* table;

    // QGraphicsItem interface
};

#endif // GRAPHMLITEM_H
