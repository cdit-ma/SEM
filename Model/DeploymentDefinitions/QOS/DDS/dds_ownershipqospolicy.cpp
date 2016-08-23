#include "dds_ownershipqospolicy.h"

DDS_OwnershipQosPolicy::DDS_OwnershipQosPolicy():Node(NK_QOS_DDS_POLICY_OWNERSHIP)
{
    setNodeType(NT_QOS_DDS_POLICY);
}

bool DDS_OwnershipQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_OwnershipQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
