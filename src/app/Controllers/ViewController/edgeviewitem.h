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
    EdgeViewItem(ViewController* controller, EDGE_KIND kind, const QString& label);
    ~EdgeViewItem() final;

    EDGE_KIND getEdgeKind() const;
    
    NodeViewItem* getSource();
    NodeViewItem* getDestination();
    NodeViewItem* getParentItem();
    
    int getSourceID();
    int getDestinationID();
    void disconnectEdge();
    
protected:
    void Disconnect() override;
    
private:
    NodeViewItem* source = nullptr;
    NodeViewItem* destination = nullptr;
    
    EDGE_KIND edgeKind;

    int sID = -1;
    int dID = -1;
};

#endif // EDGEVIEWITEM_H
