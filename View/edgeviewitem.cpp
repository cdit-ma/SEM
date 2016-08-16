#include "edgeviewitem.h"
#include "nodeviewitem.h"


EdgeViewItem::EdgeViewItem(ViewController *controller, int ID, NodeViewItem *parent, NodeViewItem *src, NodeViewItem *dst, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties):ViewItem(controller, ID, EK_EDGE, kind, data, properties)
{
    this->parent = parent;
    this->source = src;
    this->destination = dst;
}

EdgeViewItem::~EdgeViewItem()
{

}

NodeViewItem *EdgeViewItem::getSource()
{
    return source;
}

NodeViewItem *EdgeViewItem::getDestination()
{
    return destination;
}

NodeViewItem *EdgeViewItem::getParentItem()
{
    return parent;
}

int EdgeViewItem::getSourceID()
{
    int ID = -1;
    return ID;
}

int EdgeViewItem::getDestinationID()
{
    int ID = -1;
    return ID;
}
