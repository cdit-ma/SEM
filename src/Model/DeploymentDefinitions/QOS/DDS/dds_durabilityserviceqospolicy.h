#ifndef DDS_DURABILITYSERVICEQOSPOLICY_H
#define DDS_DURABILITYSERVICEQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_DurabilityServiceQosPolicy: public Node 
{
    Q_OBJECT
protected:
	DDS_DurabilityServiceQosPolicy(EntityFactory* factory);
	DDS_DurabilityServiceQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_DURABILITYSERVICEQOSPOLICY_H


