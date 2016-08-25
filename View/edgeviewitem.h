#ifndef EDGEVIEWITEM_H
#define EDGEVIEWITEM_H

#include "viewitem.h"
#include "../Model/edge.h"

class NodeViewItem;
class EdgeViewItem: public ViewItem
{
    Q_OBJECT
public:
    EdgeViewItem(ViewController* controller, int ID, NodeViewItem* src, NodeViewItem* dst, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties);
    ~EdgeViewItem();

    Edge::EDGE_CLASS getEdgeKind() const;
    NodeViewItem* getSource();
    NodeViewItem* getDestination();
    NodeViewItem* getParentItem();
    int getSourceID();
    int getDestinationID();
private:
    NodeViewItem* source;
    NodeViewItem* destination;
    Edge::EDGE_CLASS edgeKind;

    int sID;
    int dID;
};
#endif // VIEWITEM_H
