#include "dds_partitionqospolicy.h"

DDS_PartitionQosPolicy::DDS_PartitionQosPolicy():Node(NK_QOS_DDS_POLICY_PARTITION)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_PartitionQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_PartitionQosPolicy::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return false;
}
