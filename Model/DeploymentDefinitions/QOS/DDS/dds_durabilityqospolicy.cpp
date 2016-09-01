#include "dds_durabilityqospolicy.h"

DDS_DurabilityQosPolicy::DDS_DurabilityQosPolicy():Node(NK_QOS_DDS_POLICY_DURABILITY)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_DurabilityQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DurabilityQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
