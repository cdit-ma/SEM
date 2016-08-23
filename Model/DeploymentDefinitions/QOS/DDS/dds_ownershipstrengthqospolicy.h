#ifndef DDS_OWNERSHIPSTRENGTHQOSPOLICY_H
#define DDS_OWNERSHIPSTRENGTHQOSPOLICY_H
#include "../../../node.h"

class DDS_OwnershipStrengthQosPolicy: public Node
{
    Q_OBJECT
public:
    DDS_OwnershipStrengthQosPolicy();
    bool canAdoptChild(Node* node);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};
#endif // DDS_OWNERSHIPSTRENGTHQOSPOLICY_H


