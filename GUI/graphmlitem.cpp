#include "graphmlitem.h"
#include "../Model/graphml.h"
#include "attributetablemodel.h"
#include <QObject>
#include <QDebug>

GraphMLItem::GraphMLItem(GraphML* graph):QObject(0), QGraphicsItem(0)
{
    attachedGraph = graph;
    table = new AttributeTableModel(this);
}

GraphMLItem::~GraphMLItem()
{
    if(table){
        delete table;
        table = 0;
    }

}

GraphML *GraphMLItem::getGraphML()
{
    return attachedGraph;
}

AttributeTableModel *GraphMLItem::getAttributeTable()
{
    return table;
}
