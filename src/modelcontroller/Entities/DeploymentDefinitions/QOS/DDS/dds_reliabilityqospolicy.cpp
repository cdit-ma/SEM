#include "dds_reliabilityqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_RELIABILITY;
const static QString kind_string = "DDS_ReliabilityQosPolicy";

void DDS_ReliabilityQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_ReliabilityQosPolicy(broker, is_temp_node);
    });
}

DDS_ReliabilityQosPolicy::DDS_ReliabilityQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    QList<QVariant> values;
    values << "BEST_EFFORT_RELIABILITY_QOS";
    values << "RELIABLE_RELIABILITY_QOS";
    broker.AttachData(this, "label", QVariant::String, "reliability", true);
    broker.AttachData(this, "qos_dds_max_blocking_time_sec", QVariant::String, "0", false);
    broker.AttachData(this, "qos_dds_max_blocking_time_nanosec", QVariant::String, "100000000", false);
    auto dds_kinds_data = broker.AttachData(this, "qos_dds_kind", QVariant::String, "0", false);
    dds_kinds_data->addValidValues(values);
}