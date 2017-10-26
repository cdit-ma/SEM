#include "dds_ownershipqospolicy.h"

DDS_OwnershipQosPolicy::DDS_OwnershipQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_OWNERSHIP, "DDS_OwnershipQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_OWNERSHIP;
	QString kind_string = "DDS_OwnershipQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_OwnershipQosPolicy();});

    QList<QVariant> values;
    values << "SHARED_OWNERSHIP_QOS";
    values << "EXCLUSIVE_OWNERSHIP_QOS";

    RegisterValidDataValues(factory, node_kind, "qos_dds_kind", QVariant::String, values);

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "ownership");
    RegisterDefaultData(factory, node_kind, "qos_dds_kind", QVariant::String, false, values.first());
}

DDS_OwnershipQosPolicy::DDS_OwnershipQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_OWNERSHIP)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}

bool DDS_OwnershipQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_OwnershipQosPolicy::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}