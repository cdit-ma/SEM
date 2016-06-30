#include "edgeitem.h"

EdgeItemNew::EdgeItemNew(EdgeViewItem* edgeViewItem, NodeItemNew * parent, NodeItemNew* source, NodeItemNew* destination, KIND edge_kind):EntityItemNew(viewItem, parent, EntityItemNew::EDGE)
{
    this->edgeViewItem = edgeViewItem;
    this->sourceItem = source;
    this->destinationItem = destination;
    this->edge_kind = edge_kind;

    //TODO
    parent->addEdgeItem(this);
}

EdgeItemNew::KIND EdgeItemNew::getEdgeItemKind()
{
    return edge_kind;
}

NodeItemNew *EdgeItemNew::getParentItem()
{
    return (NodeItemNew*)getParent();
}

NodeItemNew *EdgeItemNew::getSourceItem()
{
    return sourceItem;
}

NodeItemNew *EdgeItemNew::getDestinationItem()
{
    return destinationItem;
}
