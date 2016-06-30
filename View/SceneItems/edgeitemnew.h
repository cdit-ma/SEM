#ifndef EDGEITEMNEW_H
#define EDGEITEMNEW_H

#include "../edgeviewitem.h"

#include "entityitemnew.h"

class NodeItemNew;

class EdgeItemNew : public EntityItemNew{
    Q_OBJECT

public:
    enum KIND{DEFAULT};

    EdgeItemNew(EdgeViewItem* edgeViewItem, NodeItemNew* parent, NodeItemNew* source, NodeItemNew* destination, KIND edge_kind = DEFAULT);
    ~EdgeItemNew();
    KIND getEdgeItemKind();

    NodeItemNew* getParentItem();
    NodeItemNew* getSourceItem();
    NodeItemNew* getDestinationItem();

private:
    KIND edge_kind;
    EdgeViewItem* edgeViewItem;
    NodeItemNew* parentItem;
    NodeItemNew* sourceItem;
    NodeItemNew* destinationItem;
};

#endif // EDGEITEMNEW_H
