#include "edgeitem.h"

EdgeItemNew::EdgeItemNew(EdgeViewItem* edgeViewItem, EntityItemNew* parent, NodeItemNew* source, NodeItemNew* destination, KIND edge_kind):EntityItemNew(viewItem, parent, EntityItemNew::EDGE)
{
    this->edgeViewItem = edgeViewItem;

    //TODO
    parent->addEdgeItem(this);



}
