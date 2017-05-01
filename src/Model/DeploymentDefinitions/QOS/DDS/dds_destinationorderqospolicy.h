#ifndef DDS_DESTINATIONORDERQOSPOLICY_H
#define DDS_DESTINATIONORDERQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_DestinationOrderQosPolicy: public Node 
{
    Q_OBJECT
protected:
	DDS_DestinationOrderQosPolicy(EntityFactory* factory);
	DDS_DestinationOrderQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_DESTINATIONORDERQOSPOLICY_H


