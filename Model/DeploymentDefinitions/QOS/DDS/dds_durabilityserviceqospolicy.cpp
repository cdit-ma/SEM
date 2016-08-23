#include "dds_durabilityserviceqospolicy.h"

DDS_DurabilityServiceQosPolicy::DDS_DurabilityServiceQosPolicy():Node(NK_QOS_DDS_POLICY_DURABILITYSERVICE)
{
    setNodeType(NT_QOS_DDS_POLICY);
}

bool DDS_DurabilityServiceQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DurabilityServiceQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
