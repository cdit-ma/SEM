#include "dds_durabilityserviceqospolicy.h"

DDS_DurabilityServiceQosPolicy::DDS_DurabilityServiceQosPolicy():Node(NK_QOS_DDS_POLICY_DURABILITYSERVICE)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_DurabilityServiceQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DurabilityServiceQosPolicy::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return false;
}
