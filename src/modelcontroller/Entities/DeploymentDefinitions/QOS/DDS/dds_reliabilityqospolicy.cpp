#include "dds_reliabilityqospolicy.h"


DDS_ReliabilityQosPolicy::DDS_ReliabilityQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_RELIABILITY, "DDS_ReliabilityQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_RELIABILITY;
	QString kind_string = "DDS_ReliabilityQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_ReliabilityQosPolicy();});

    QList<QVariant> values;
    values << "BEST_EFFORT_RELIABILITY_QOS";
    values << "RELIABLE_RELIABILITY_QOS";

    RegisterValidDataValues(factory, node_kind, "qos_dds_kind", QVariant::String, values);

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "reliability");
    RegisterDefaultData(factory, node_kind, "qos_dds_kind", QVariant::String, false, values.first());
    RegisterDefaultData(factory, node_kind, "qos_dds_max_blocking_time_sec", QVariant::String, false, "0");
    RegisterDefaultData(factory, node_kind, "qos_dds_max_blocking_time_nanosec", QVariant::String, false, "100000000");
}

DDS_ReliabilityQosPolicy::DDS_ReliabilityQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_RELIABILITY)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}