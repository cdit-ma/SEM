#include "dds_ownershipstrengthqospolicy.h"

DDS_OwnershipStrengthQosPolicy::DDS_OwnershipStrengthQosPolicy():Node(NK_QOS_DDS_POLICY_OWNERSHIPSTRENGTH)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_OwnershipStrengthQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_OwnershipStrengthQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
