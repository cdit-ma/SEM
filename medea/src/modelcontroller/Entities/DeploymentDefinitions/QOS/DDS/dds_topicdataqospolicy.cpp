#include "dds_topicdataqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_TOPICDATA;
const static QString kind_string = "DDS_TopicDataQosPolicy";

void DDS_TopicDataQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_TopicDataQosPolicy(broker, is_temp_node);
    });
}

DDS_TopicDataQosPolicy::DDS_TopicDataQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "topic_data");
    broker.AttachData(this, KeyName::QosDdsStrValue, QVariant::String, ProtectedState::UNPROTECTED);
}