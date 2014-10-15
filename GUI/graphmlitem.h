#ifndef GRAPHMLITEM_H
#define GRAPHMLITEM_H
#include <QObject>
#include "../Model/graphml.h"


class AttributeTableModel;
class GraphMLItem: public QObject
{
    Q_OBJECT
public:
    GraphMLItem(GraphML* attachedGraph);
    ~GraphMLItem();
    GraphML* getGraphML();
    AttributeTableModel* getAttributeTable();
signals:
    void setItemSelected(GraphML*, bool setSelected=true);
    void actionTriggered(QString actionName);
    void updateGraphMLData(GraphML* element, QString keyName, QString value);

    void destructGraphMLData(GraphML* element, QString keyName);
    void constructGraphMLData(GraphML* element, QString keyName);
private:
    GraphML* attachedGraph;
    AttributeTableModel* table;
};

#endif // GRAPHMLITEM_H
