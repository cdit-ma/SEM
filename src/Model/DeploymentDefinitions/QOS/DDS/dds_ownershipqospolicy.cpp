#include "dds_ownershipqospolicy.h"

DDS_OwnershipQosPolicy::DDS_OwnershipQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_OWNERSHIP)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "ownership");
    updateDefaultData("qos_dds_kind", QVariant::String, false, "SHARED_OWNERSHIP_QOS");
    
    auto values = QStringList();
    values << "SHARED_OWNERSHIP_QOS";
    values << "EXCLUSIVE_OWNERSHIP_QOS";
}

bool DDS_OwnershipQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_OwnershipQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}