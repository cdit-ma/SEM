#include "dds_groupdataqospolicy.h"

DDS_GroupDataQosPolicy::DDS_GroupDataQosPolicy():Node(NK_QOS_DDS_POLICY_GROUPDATA)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "group_data");
    updateDefaultData("qos_dds_str_value", QVariant::String, false);
}

bool DDS_GroupDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_GroupDataQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
