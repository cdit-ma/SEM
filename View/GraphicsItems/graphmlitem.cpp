#include "graphmlitem.h"
#include "../../Model/graphml.h"
#include "../Table/attributetablemodel.h"
#include "../nodeview.h"
#include <QObject>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QDebug>

GraphMLItem::GraphMLItem(GraphML *attachedGraph, GraphMLItem::GUI_KIND kind)
{
    this->attachedGraph = attachedGraph;
    table = new AttributeTableModel(this);
    nodeView = 0;
    this->kind = kind;
    IS_DELETING = false;
    if(attachedGraph){
        this->ID = attachedGraph->getID();
    }
    setFlag(QGraphicsItem::ItemIsSelectable, false);
}

void GraphMLItem::detach()
{
    IS_DELETING = true;
    delete table;
    table = 0;
    attachedGraph = 0;
}

bool GraphMLItem::isDeleting()
{
    return IS_DELETING;
}

GraphMLItem::~GraphMLItem()
{
    if(table){
        delete table;
        table = 0;
    }
}

QString GraphMLItem::getGraphMLDataValue(QString key)
{
    if(getGraphML() && !getGraphML()->isDeleting()){
        GraphMLData* data = getGraphML()->getData(key);
        if(!data->isDeleting() && data->getParent() == getGraphML()){
            return data->getValue();
        }
    }
    return QString();
}

GraphML *GraphMLItem::getGraphML()
{

    return attachedGraph;

}

AttributeTableModel *GraphMLItem::getAttributeTable()
{
    return table;
}

void GraphMLItem::setNodeView(NodeView *view)
{
    nodeView = view;
}

NodeView *GraphMLItem::getNodeView()
{
    return nodeView;
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
