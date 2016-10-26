#ifndef DDS_OWNERSHIPQOSPOLICY_H
#define DDS_OWNERSHIPQOSPOLICY_H
#include "../../../node.h"

class DDS_OwnershipQosPolicy: public Node
{
    Q_OBJECT
public:
    DDS_OwnershipQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DDS_OWNERSHIPQOSPOLICY_H


