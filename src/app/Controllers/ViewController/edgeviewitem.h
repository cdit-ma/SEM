#ifndef EDGEVIEWITEM_H
#define EDGEVIEWITEM_H

#include "viewitem.h"
#include "../../../modelcontroller/edgekinds.h"

class NodeViewItem;
class EdgeViewItem: public ViewItem
{
    Q_OBJECT
public:
    EdgeViewItem(ViewController* controller, int ID, NodeViewItem* src, NodeViewItem* dst, EDGE_KIND kind);
    EdgeViewItem(ViewController* controller, EDGE_KIND kind, QString label);
    ~EdgeViewItem();

    EDGE_KIND getEdgeKind() const;
    NodeViewItem* getSource();
    NodeViewItem* getDestination();
    NodeViewItem* getParentItem();
    int getSourceID();
    int getDestinationID();
    void disconnectEdge();
protected:
    void Disconnect();
private:
    NodeViewItem* source;
    NodeViewItem* destination;
    EDGE_KIND edgeKind;

    int sID;
    int dID;
};
#endif // VIEWITEM_H
