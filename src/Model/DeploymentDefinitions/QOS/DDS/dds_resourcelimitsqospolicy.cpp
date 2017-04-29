#include "dds_resourcelimitsqospolicy.h"
DDS_ResourceLimitsQosPolicy::DDS_ResourceLimitsQosPolicy():Node(NK_QOS_DDS_POLICY_RESOURCELIMITS)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "resource_limits");
    updateDefaultData("qos_dds_max_samples", QVariant::String, false, "LENGTH_UNLIMITED");
    updateDefaultData("qos_dds_max_instances", QVariant::String, false, "LENGTH_UNLIMITED");
    updateDefaultData("qos_dds_max_samples_per_instance", QVariant::String, false, "LENGTH_UNLIMITED");
}

bool DDS_ResourceLimitsQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_ResourceLimitsQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
