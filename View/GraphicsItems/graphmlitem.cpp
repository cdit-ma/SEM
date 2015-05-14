#include "graphmlitem.h"
#include "../../Model/graphml.h"
#include "../Table/attributetablemodel.h"
#include <QObject>
#include <QDebug>

GraphMLItem::GraphMLItem(GraphML *attachedGraph, GraphMLItem::GUI_KIND kind)
{
    this->attachedGraph = attachedGraph;
    table = new AttributeTableModel(this);
    this->kind = kind;
    if(attachedGraph){
        this->ID = attachedGraph->getID();
    }
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

bool GraphMLItem::isNodeItem()
{
    return kind == NODE_ITEM;
}

bool GraphMLItem::isEdgeItem()
{
    return kind == NODE_EDGE;
}

QString GraphMLItem::getID()
{
    return this->ID;
}
