#include "dds_partitionqospolicy.h"

DDS_PartitionQosPolicy::DDS_PartitionQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_PARTITION, "DDS_PartitionQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_PARTITION;
	QString kind_string = "DDS_PartitionQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_PartitionQosPolicy();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "partition");
    RegisterDefaultData(factory, node_kind, "qos_dds_name", QVariant::String, false);
}

DDS_PartitionQosPolicy::DDS_PartitionQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_PARTITION)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}
