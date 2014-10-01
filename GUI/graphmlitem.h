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
    GraphML* getGraphML();
    AttributeTableModel* getAttributeTable();
signals:
    void setItemSelected(GraphML*, bool setSelected=true);
    void actionTriggered(QString actionName);
    void updateGraphMLDataValue(GraphML* element, QString keyName, QString value);

private:
    GraphML* attachedGraph;
    AttributeTableModel* table;
};

#endif // GRAPHMLITEM_H
