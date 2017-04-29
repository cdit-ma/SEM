#include "dds_ownershipstrengthqospolicy.h"

DDS_OwnershipStrengthQosPolicy::DDS_OwnershipStrengthQosPolicy():Node(NK_QOS_DDS_POLICY_OWNERSHIPSTRENGTH)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
    
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
