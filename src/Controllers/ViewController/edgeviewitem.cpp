#include "edgeviewitem.h"
#include "nodeviewitem.h"


EdgeViewItem::EdgeViewItem(ViewController *controller, int ID, NodeViewItem *src, NodeViewItem *dst, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties):ViewItem(controller, ID, EK_EDGE, kind, data, properties)
{
    sID = -1;
    dID = -1;

    this->source = src;
    if(src){
        this->sID = src->getID();
    }
    this->destination = dst;
    if(dst){
        this->dID = dst->getID();
    }

    edgeKind = EDGE_KIND::NONE;
    if(properties.contains("kind")){
        edgeKind = (EDGE_KIND) properties["kind"].toInt();
    }
}

EdgeViewItem::EdgeViewItem(ViewController *controller, EDGE_KIND kind): ViewItem(controller)
{
    edgeKind = kind;
}

EdgeViewItem::~EdgeViewItem()
{
    disconnectEdge();
}

EDGE_KIND EdgeViewItem::getEdgeKind() const
{
    return edgeKind;
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
    if(ViewItem::getParentItem()){
        return (NodeViewItem*) ViewItem::getParentItem();
    }
    return 0;
}

int EdgeViewItem::getSourceID()
{
    return sID;
}

int EdgeViewItem::getDestinationID()
{
    return dID;
}

void EdgeViewItem::disconnectEdge()
{
    if(source){
        source->removeEdgeItem(this);
        source = 0;
    }
    if(destination){
        destination->removeEdgeItem(this);
        destination = 0;
    }
}
