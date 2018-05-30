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
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED, "liveliness");
    broker.AttachData(this, "qos_dds_lease_duration_sec", QVariant::String, ProtectedState::UNPROTECTED, "DURATION_INFINITE_SEC");
    broker.AttachData(this, "qos_dds_lease_duration_nanosec", QVariant::String, ProtectedState::UNPROTECTED, "DURATION_INFINITE_NSEC");
    auto dds_kind_data = broker.AttachData(this, "qos_dds_kind", QVariant::String, ProtectedState::UNPROTECTED);
    dds_kind_data->addValidValues({"AUTOMATIC_LIVELINESS_QOS", "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS", "MANUAL_BY_TOPIC_LIVELINESS_QOS"});
}