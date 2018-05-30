#include "dds_userdataqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_USERDATA;
const static QString kind_string = "DDS_UserDataQosPolicy";

void DDS_UserDataQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_UserDataQosPolicy(broker, is_temp_node);
    });
}

DDS_UserDataQosPolicy::DDS_UserDataQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED, "user_data");
    broker.AttachData(this, "qos_dds_str_value", QVariant::String, ProtectedState::UNPROTECTED);
}