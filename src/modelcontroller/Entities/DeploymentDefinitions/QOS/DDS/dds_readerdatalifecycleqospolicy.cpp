#include "dds_readerdatalifecycleqospolicy.h"

DDS_ReaderDataLifecycleQosPolicy::DDS_ReaderDataLifecycleQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_READERDATALIFECYCLE, "DDS_ReaderDataLifecycleQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_READERDATALIFECYCLE;
	QString kind_string = "DDS_ReaderDataLifecycleQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_ReaderDataLifecycleQosPolicy();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "reader_data_lifecycle");
    RegisterDefaultData(factory, node_kind, "qos_dds_autopurge_nowriter_samples_delay_sec", QVariant::String, false, "DURATION_INFINITE_SEC");
    RegisterDefaultData(factory, node_kind, "qos_dds_autopurge_nowriter_samples_delay_nanosec", QVariant::String, false, "DURATION_INFINITE_NSEC");
    RegisterDefaultData(factory, node_kind, "qos_dds_autopurge_disposed_samples_delay_sec", QVariant::String, false, "DURATION_INFINITE_SEC");
    RegisterDefaultData(factory, node_kind, "qos_dds_autopurge_disposed_samples_delay_nanosec", QVariant::String, false, "DURATION_INFINITE_NSEC");
}

DDS_ReaderDataLifecycleQosPolicy::DDS_ReaderDataLifecycleQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_READERDATALIFECYCLE)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}