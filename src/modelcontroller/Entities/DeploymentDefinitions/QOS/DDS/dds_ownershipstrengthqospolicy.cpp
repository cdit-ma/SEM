#include "dds_ownershipstrengthqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_OWNERSHIPSTRENGTH;
const static QString kind_string = "DDS_OwnershipStrengthQosPolicy";

void DDS_OwnershipStrengthQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_OwnershipStrengthQosPolicy(broker, is_temp_node);
    });
}

DDS_OwnershipStrengthQosPolicy::DDS_OwnershipStrengthQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "ownership_strength");
    broker.AttachData(this, KeyName::QosDdsIntValue, QVariant::Int, ProtectedState::UNPROTECTED, 0);
}