#ifndef DDS_DURABILITYQOSPOLICY_H
#define DDS_DURABILITYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_DurabilityQosPolicy: public Node 
{
    Q_OBJECT
protected:
	DDS_DurabilityQosPolicy(EntityFactory* factory);
	DDS_DurabilityQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_DURABILITYQOSPOLICY_H


