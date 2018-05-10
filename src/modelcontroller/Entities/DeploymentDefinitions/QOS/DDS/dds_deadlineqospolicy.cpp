#include "dds_deadlineqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_DEADLINE;
const static QString kind_string = "DDS_DeadlineQosPolicy";

void DDS_DeadlineQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_DeadlineQosPolicy(broker, is_temp_node);
    });
}

DDS_DeadlineQosPolicy::DDS_DeadlineQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);



    
    //Setup Data
    broker.AttachData(this, "label", QVariant::String, "deadline", true);
    broker.AttachData(this, "qos_dds_period_sec", QVariant::String, "DURATION_INFINITE_SEC", false);
    broker.AttachData(this, "qos_dds_period_nanosec", QVariant::String, "DURATION_INFINITE_NSEC", false);
}