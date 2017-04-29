#include "dds_timebasedfilterqospolicy.h"

DDS_TimeBasedFilterQosPolicy::DDS_TimeBasedFilterQosPolicy():Node(NK_QOS_DDS_POLICY_TIMEBASEDFILTER)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "time_based_filter");
    updateDefaultData("qos_dds_minimum_separation_sec", QVariant::String, false, "0");
    updateDefaultData("qos_dds_minimum_separation_nanosec", QVariant::String, false, "0");
}

bool DDS_TimeBasedFilterQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_TimeBasedFilterQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}