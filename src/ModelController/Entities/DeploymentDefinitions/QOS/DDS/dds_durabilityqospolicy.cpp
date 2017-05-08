#include "dds_durabilityqospolicy.h"

DDS_DurabilityQosPolicy::DDS_DurabilityQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_DURABILITY, "DDS_DurabilityQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_DURABILITY;
	QString kind_string = "DDS_DurabilityQosPolicy";
	
    RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_DurabilityQosPolicy();});

    QList<QVariant> values;
    values << "VOLATILE_DURABILITY_QOS";
    values << "TRANSIENT_LOCAL_DURABILITY_QOS";
    values << "TRANSIENT_DURABILITY_QOS";
    values << "PERSISTENT_DURABILITY_QOS";

    RegisterValidDataValues(factory, node_kind, "qos_dds_kind", QVariant::String, values);

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "durability");
    RegisterDefaultData(factory, node_kind, "qos_dds_kind", QVariant::String, false, values.first());
};

DDS_DurabilityQosPolicy::DDS_DurabilityQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_DURABILITY)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}

bool DDS_DurabilityQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DurabilityQosPolicy::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
