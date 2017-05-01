#include "dds_ownershipqospolicy.h"

#include "../../../entityfactory.h"
DDS_OwnershipQosPolicy::DDS_OwnershipQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_OWNERSHIP, "DDS_OwnershipQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_OWNERSHIP;
	QString kind_string = "DDS_OwnershipQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_OwnershipQosPolicy();});
};
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