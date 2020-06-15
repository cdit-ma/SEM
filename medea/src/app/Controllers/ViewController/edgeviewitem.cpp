#include "edgeviewitem.h"
#include "nodeviewitem.h"
#include "../../../modelcontroller/entityfactory.h"

EdgeViewItem::EdgeViewItem(ViewController* controller, int ID, NodeViewItem* src, NodeViewItem* dst, EDGE_KIND kind)
	: ViewItem(controller, ID, GRAPHML_KIND::EDGE)
{
    source = src;
    if (src) {
        sID = src->getID();
    }
    destination = dst;
    if (dst) {
        dID = dst->getID();
    }
    edgeKind = kind;
}

EdgeViewItem::EdgeViewItem(ViewController* controller, EDGE_KIND kind, const QString& label)
	: ViewItem(controller, GRAPHML_KIND::EDGE)
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

NodeViewItem* EdgeViewItem::getSource()
{
    return source;
}

NodeViewItem* EdgeViewItem::getDestination()
{
    return destination;
}

NodeViewItem* EdgeViewItem::getParentItem()
{
    if (ViewItem::getParentItem()) {
        return (NodeViewItem*) ViewItem::getParentItem();
    }
    return nullptr;
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
    if (source) {
        source->removeEdgeItem(this);
        source = nullptr;
    }
    if (destination) {
        destination->removeEdgeItem(this);
        destination = nullptr;
    }
}

void EdgeViewItem::Disconnect()
{
    disconnectEdge();
    ViewItem::Disconnect();
}