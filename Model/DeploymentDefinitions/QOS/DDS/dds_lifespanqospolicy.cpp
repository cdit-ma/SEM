#include "dds_lifespanqospolicy.h"

DDS_LifespanQosPolicy::DDS_LifespanQosPolicy():Node(NK_QOS_DDS_POLICY_LIFESPAN)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_LifespanQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_LifespanQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
