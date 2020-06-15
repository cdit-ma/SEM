#include "dds_entityfactoryqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_ENTITYFACTORY;
const static QString kind_string = "DDS_EntityFactoryQosPolicy";

void DDS_EntityFactoryQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_EntityFactoryQosPolicy(broker, is_temp_node);
    });
}

DDS_EntityFactoryQosPolicy::DDS_EntityFactoryQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED,  "entity_factory");
    broker.AttachData(this, KeyName::QosDdsAutoEnableCreatedEntities, QVariant::Bool, ProtectedState::UNPROTECTED, true);
}