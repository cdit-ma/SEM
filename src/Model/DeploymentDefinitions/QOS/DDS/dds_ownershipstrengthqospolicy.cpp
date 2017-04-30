#include "dds_ownershipstrengthqospolicy.h"

DDS_OwnershipStrengthQosPolicy::DDS_OwnershipStrengthQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_OWNERSHIPSTRENGTH)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
    
    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "ownership_strength");
    updateDefaultData("qos_dds_int_value", QVariant::Int, false, 0);
}

bool DDS_OwnershipStrengthQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_OwnershipStrengthQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
