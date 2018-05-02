#include "dds_durabilityserviceqospolicy.h"

DDS_DurabilityServiceQosPolicy::DDS_DurabilityServiceQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_DURABILITYSERVICE, "DDS_DurabilityServiceQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_DURABILITYSERVICE;
	QString kind_string = "DDS_DurabilityServiceQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_DurabilityServiceQosPolicy();});

    QList<QVariant> values;
    values << "KEEP_LAST_HISTORY_QOS";
    values << "KEEP_ALL_HISTORY_QOS";

    RegisterValidDataValues(factory, node_kind, "qos_dds_history_kind", QVariant::String, values);

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "durability_service");
    RegisterDefaultData(factory, node_kind, "qos_dds_service_cleanup_delay_sec", QVariant::String, false, "0");
    RegisterDefaultData(factory, node_kind, "qos_dds_service_cleanup_delay_nanosec", QVariant::String, false, "0");
    RegisterDefaultData(factory, node_kind, "qos_dds_history_kind", QVariant::String, false, values.first());

    RegisterDefaultData(factory, node_kind, "qos_dds_history_depth", QVariant::String, false, "1");
    RegisterDefaultData(factory, node_kind, "qos_dds_max_samples", QVariant::String, false, "LENGTH_UNLIMITED");
    RegisterDefaultData(factory, node_kind, "qos_dds_max_instances", QVariant::String, false, "LENGTH_UNLIMITED");
    RegisterDefaultData(factory, node_kind, "qos_dds_max_samples_per_instance", QVariant::String, false, "LENGTH_UNLIMITED");
};

DDS_DurabilityServiceQosPolicy::DDS_DurabilityServiceQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_DURABILITYSERVICE)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}