#include "dds_reliabilityqospolicy.h"

DDS_ReliabilityQosPolicy::DDS_ReliabilityQosPolicy():Node(NK_QOS_DDS_POLICY_RELIABILITY)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_ReliabilityQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_ReliabilityQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
