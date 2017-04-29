#include "dds_partitionqospolicy.h"

DDS_PartitionQosPolicy::DDS_PartitionQosPolicy():Node(NK_QOS_DDS_POLICY_PARTITION)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "partition");
    updateDefaultData("qos_dds_name", QVariant::String);
}

bool DDS_PartitionQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_PartitionQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
