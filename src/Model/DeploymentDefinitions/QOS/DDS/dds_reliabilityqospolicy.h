#ifndef DDS_RELIABILITYQOSPOLICY_H
#define DDS_RELIABILITYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_ReliabilityQosPolicy: public Node 
{
    Q_OBJECT
protected:
	DDS_ReliabilityQosPolicy(EntityFactory* factory);
	DDS_ReliabilityQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_RELIABILITYQOSPOLICY_H


