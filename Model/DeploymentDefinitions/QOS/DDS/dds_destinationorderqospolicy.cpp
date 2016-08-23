#include "dds_destinationorderqospolicy.h"

DDS_DestinationOrderQosPolicy::DDS_DestinationOrderQosPolicy():Node(NK_QOS_DDS_POLICY_DESTINATIONORDER)
{
    setNodeType(NT_QOS_DDS_POLICY);
}

bool DDS_DestinationOrderQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DestinationOrderQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
