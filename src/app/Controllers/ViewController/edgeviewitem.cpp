#include "edgeviewitem.h"
#include "nodeviewitem.h"
#include "../../../modelcontroller/entityfactory.h"

EdgeViewItem::EdgeViewItem(ViewController* controller, int ID, NodeViewItem* src, NodeViewItem* dst, EDGE_KIND kind):ViewItem(controller, ID, GRAPHML_KIND::EDGE)
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

    edgeKind = kind;
}

EdgeViewItem::EdgeViewItem(ViewController *controller, EDGE_KIND kind, QString label): ViewItem(controller, GRAPHML_KIND::EDGE)
{
    edgeKind = kind;
    changeData(KeyName::Kind, EntityFactory::getEdgeKindString(kind));
    changeData(KeyName::Label, label);
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
