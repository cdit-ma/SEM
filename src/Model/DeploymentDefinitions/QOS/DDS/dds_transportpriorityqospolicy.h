#ifndef DDS_TRANSPORTPRIORITYQOSPOLICY_H
#define DDS_TRANSPORTPRIORITYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_TransportPriorityQosPolicy: public Node 
{
    Q_OBJECT
protected:
	DDS_TransportPriorityQosPolicy(EntityFactory* factory);
	DDS_TransportPriorityQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_TRANSPORTPRIORITYQOSPOLICY_H


