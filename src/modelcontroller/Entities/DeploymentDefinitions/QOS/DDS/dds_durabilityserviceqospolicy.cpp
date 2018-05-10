#include "dds_durabilityserviceqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_DURABILITYSERVICE;
const static QString kind_string = "DDS_DurabilityServiceQosPolicy";

void DDS_DurabilityServiceQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_DurabilityServiceQosPolicy(broker, is_temp_node);
    });
}

DDS_DurabilityServiceQosPolicy::DDS_DurabilityServiceQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
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
    broker.AttachData(this, "label", QVariant::String, "durability_service", true);
    broker.AttachData(this, "qos_dds_service_cleanup_delay_sec", QVariant::String, "0", false);
    broker.AttachData(this, "qos_dds_service_cleanup_delay_nanosec", QVariant::String, "0", false);
    broker.AttachData(this, "qos_dds_history_depth", QVariant::String, "1", false);
    broker.AttachData(this, "qos_dds_max_samples", QVariant::String, "LENGTH_UNLIMITED", false);
    broker.AttachData(this, "qos_dds_max_instances", QVariant::String, "LENGTH_UNLIMITED", false);
    broker.AttachData(this, "qos_dds_max_samples_per_instance", QVariant::String, "LENGTH_UNLIMITED", false);
    auto dds_kind_data = broker.AttachData(this, "qos_dds_history_kind", QVariant::String, values.first(), false);
    dds_kind_data->addValidValues(values);
}