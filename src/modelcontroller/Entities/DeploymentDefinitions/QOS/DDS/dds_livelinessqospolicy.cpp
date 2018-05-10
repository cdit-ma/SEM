#include "dds_livelinessqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_LIVELINESS;
const static QString kind_string = "DDS_LivelinessQosPolicy";

void DDS_LivelinessQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_LivelinessQosPolicy(broker, is_temp_node);
    });
}

DDS_LivelinessQosPolicy::DDS_LivelinessQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);




    //Setup Data
    QList<QVariant> values;
    values << "AUTOMATIC_LIVELINESS_QOS";
    values << "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS";
    values << "MANUAL_BY_TOPIC_LIVELINESS_QOS";
    broker.AttachData(this, "label", QVariant::String, "liveliness", true);
    broker.AttachData(this, "qos_dds_lease_duration_sec", QVariant::String, "DURATION_INFINITE_SEC", false);
    broker.AttachData(this, "qos_dds_lease_duration_nanosec", QVariant::String, "DURATION_INFINITE_NSEC", false);
    auto dds_kind_data = broker.AttachData(this, "qos_dds_kind", QVariant::String, values.first(), false);
    dds_kind_data->addValidValues(values);
}