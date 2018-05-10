#include "memberinstance.h"
#include "aggregateinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::MEMBER_INSTANCE;
const QString kind_string = "MemberInstance";

void MemberInstance::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new MemberInstance(broker, is_temp_node);
    });
}

MemberInstance::MemberInstance(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    addInstancesDefinitionKind(NODE_KIND::MEMBER);
    setChainableDefinition();



    //Setup Data
    broker.AttachData(this, "index", QVariant::Int, -1, true);
    broker.AttachData(this, "type", QVariant::String, "", true);
}


void MemberInstance::parentSet(Node* parent){
    AggregateInstance::ParentSet(this);
    DataNode::parentSet(parent);
}