#include "edgeviewitem.h"
#include "nodeviewitem.h"


EdgeViewItem::EdgeViewItem(ViewController *controller, int ID, NodeViewItem *parent, NodeViewItem *src, NodeViewItem *dst, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties):ViewItem(controller, ID, EK_EDGE, kind, data, properties)
{
    sID = -1;
    dID = -1;
    this->parent = parent;

    this->source = src;
    if(src){
        this->sID = src->getID();
    }
    this->destination = dst;
    if(dst){
        this->dID = dst->getID();
    }

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
    return sID;
}

int EdgeViewItem::getDestinationID()
{
    return dID;
}
