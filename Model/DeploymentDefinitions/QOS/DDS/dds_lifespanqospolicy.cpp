#include "dds_lifespanqospolicy.h"

DDS_LifespanQosPolicy::DDS_LifespanQosPolicy():Node(NK_QOS_DDS_POLICY_LIFESPAN)
{
    setNodeType(NT_QOS_DDS_POLICY);
}

bool DDS_LifespanQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_LifespanQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
