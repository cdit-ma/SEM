#include "dds_durabilityserviceqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_DURABILITYSERVICE;
const static QString kind_string = "DDS_DurabilityServiceQosPolicy";

void DDS_DurabilityServiceQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_DurabilityServiceQosPolicy(factory, is_temp_node);
    });
}

DDS_DurabilityServiceQosPolicy::DDS_DurabilityServiceQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    QList<QVariant> values;
    values << "KEEP_LAST_HISTORY_QOS";
    values << "KEEP_ALL_HISTORY_QOS";
    factory.AttachData(this, "label", QVariant::String, "durability_service", true);
    factory.AttachData(this, "qos_dds_service_cleanup_delay_sec", QVariant::String, "0", false);
    factory.AttachData(this, "qos_dds_service_cleanup_delay_nanosec", QVariant::String, "0", false);
    factory.AttachData(this, "qos_dds_history_depth", QVariant::String, "1", false);
    factory.AttachData(this, "qos_dds_max_samples", QVariant::String, "LENGTH_UNLIMITED", false);
    factory.AttachData(this, "qos_dds_max_instances", QVariant::String, "LENGTH_UNLIMITED", false);
    factory.AttachData(this, "qos_dds_max_samples_per_instance", QVariant::String, "LENGTH_UNLIMITED", false);
    auto dds_kind_data = factory.AttachData(this, "qos_dds_history_kind", QVariant::String, values.first(), false);
    dds_kind_data->addValidValues(values);
}