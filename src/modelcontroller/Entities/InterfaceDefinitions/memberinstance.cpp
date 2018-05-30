#include "memberinstance.h"
#include "aggregateinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::MEMBER_INSTANCE;
const QString kind_string = "Member Instance";

void MemberInstance::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new MemberInstance(broker, is_temp_node);
    });
}

MemberInstance::MemberInstance(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::MEMBER);
    setChainableDefinition();

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    setLabelFunctional(false);
    broker.AttachData(this, "index", QVariant::Int, ProtectedState::PROTECTED);
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
}


void MemberInstance::parentSet(Node* parent){
    if(parent->getNodeKind() == NODE_KIND::AGGREGATE){
        setLabelFunctional(true);
    }
    AggregateInstance::ParentSet(this);
    DataNode::parentSet(parent);
}