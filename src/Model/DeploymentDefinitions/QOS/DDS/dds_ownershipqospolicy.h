#ifndef DDS_OWNERSHIPQOSPOLICY_H
#define DDS_OWNERSHIPQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_OwnershipQosPolicy: public Node
{
    Q_OBJECT
protected:
	DDS_OwnershipQosPolicy(EntityFactory* factory);
	DDS_OwnershipQosPolicy();
public:
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_OWNERSHIPQOSPOLICY_H


