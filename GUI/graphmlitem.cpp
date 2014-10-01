#include "graphmlitem.h"
#include "../Model/graphml.h"
#include "attributetablemodel.h"
#include <QObject>
#include <QDebug>

GraphMLItem::GraphMLItem(GraphML* graph): QObject(0)
{
    attachedGraph = graph;

    //Construct a table.
    table = new AttributeTableModel(this);
}

GraphMLItem::~GraphMLItem()
{
    delete table;
}

GraphML *GraphMLItem::getGraphML()
{
    return attachedGraph;
}

AttributeTableModel *GraphMLItem::getAttributeTable()
{
    return table;
}
