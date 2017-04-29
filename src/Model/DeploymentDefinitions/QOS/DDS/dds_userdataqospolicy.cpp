#include "dds_userdataqospolicy.h"

DDS_UserDataQosPolicy::DDS_UserDataQosPolicy():Node(NK_QOS_DDS_POLICY_USERDATA)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "user_data");
    updateDefaultData("qos_dds_str_value", QVariant::String);
}


bool DDS_UserDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_UserDataQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}