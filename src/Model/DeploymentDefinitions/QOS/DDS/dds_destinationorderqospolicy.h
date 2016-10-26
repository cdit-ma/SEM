#ifndef DDS_DESTINATIONORDERQOSPOLICY_H
#define DDS_DESTINATIONORDERQOSPOLICY_H
#include "../../../node.h"

class DDS_DestinationOrderQosPolicy: public Node 
{
    Q_OBJECT
public:
    DDS_DestinationOrderQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_DESTINATIONORDERQOSPOLICY_H


