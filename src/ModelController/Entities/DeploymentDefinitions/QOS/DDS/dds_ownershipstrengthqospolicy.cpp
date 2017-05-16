#include "dds_ownershipstrengthqospolicy.h"

DDS_OwnershipStrengthQosPolicy::DDS_OwnershipStrengthQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_OWNERSHIPSTRENGTH, "DDS_OwnershipStrengthQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_OWNERSHIPSTRENGTH;
	QString kind_string = "DDS_OwnershipStrengthQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_OwnershipStrengthQosPolicy();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "ownership_strength");
    RegisterDefaultData(factory, node_kind, "qos_dds_int_value", QVariant::Int, false, 0);
};

DDS_OwnershipStrengthQosPolicy::DDS_OwnershipStrengthQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_OWNERSHIPSTRENGTH)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}

bool DDS_OwnershipStrengthQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_OwnershipStrengthQosPolicy::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
