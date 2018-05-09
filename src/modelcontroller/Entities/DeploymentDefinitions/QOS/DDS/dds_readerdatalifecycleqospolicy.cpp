#include "dds_readerdatalifecycleqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_READERDATALIFECYCLE;
const static QString kind_string = "DDS_ReaderDataLifecycleQosPolicy";

void DDS_ReaderDataLifecycleQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_ReaderDataLifecycleQosPolicy(factory, is_temp_node);
    });
}

DDS_ReaderDataLifecycleQosPolicy::DDS_ReaderDataLifecycleQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "reader_data_lifecycle", true);
    factory.AttachData(this, "qos_dds_autopurge_nowriter_samples_delay_sec", QVariant::String, "DURATION_INFINITE_SEC", false);
    factory.AttachData(this, "qos_dds_autopurge_nowriter_samples_delay_nanosec", QVariant::String, "DURATION_INFINITE_NSEC", false);
    factory.AttachData(this, "qos_dds_autopurge_disposed_samples_delay_sec", QVariant::String, "DURATION_INFINITE_SEC", false);
    factory.AttachData(this, "qos_dds_autopurge_disposed_samples_delay_nanosec", QVariant::String, "DURATION_INFINITE_NSEC", false);
}