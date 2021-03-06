#include "dds_groupdataqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_GROUPDATA;
const static QString kind_string = "DDS_GroupDataQosPolicy";

void DDS_GroupDataQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_GroupDataQosPolicy(broker, is_temp_node);
    });
}

DDS_GroupDataQosPolicy::DDS_GroupDataQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "group_data");
    broker.AttachData(this, KeyName::QosDdsStrValue, QVariant::String, ProtectedState::UNPROTECTED);
}