#include "dds_destinationorderqospolicy.h"

DDS_DestinationOrderQosPolicy::DDS_DestinationOrderQosPolicy():Node(NK_QOS_DDS_POLICY_DESTINATIONORDER)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_DestinationOrderQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DestinationOrderQosPolicy::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    Q_UNUSED(edgeKind);
    Q_UNUSED(dst);
    return false;
}
