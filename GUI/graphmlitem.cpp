#include "graphmlitem.h"
#include "../Model/graphml.h"
#include "attributetablemodel.h"
#include <QObject>
#include <QDebug>

GraphMLItem::GraphMLItem(GraphML* graph):QObject(0), QGraphicsItem()
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

QVariant GraphMLItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedChange)
    {
        qCritical() << "SLEECTION";
        if (value == true){
            emit triggerSelected(getGraphML(), true);
        }else{
            emit triggerSelected(getGraphML(), false);
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

void GraphMLItem::destructGraphML()
{
    delete this;
}
