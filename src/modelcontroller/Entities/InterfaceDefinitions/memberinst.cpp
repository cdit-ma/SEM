#include "memberinst.h"
#include "aggregateinst.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::MEMBER_INST;
const QString kind_string = "Member Instance";

void MemberInst::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new MemberInst(broker, is_temp_node);
    });
}

MemberInst::MemberInst(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
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


void MemberInst::parentSet(Node* parent){
    if(parent->getNodeKind() == NODE_KIND::AGGREGATE){
        setLabelFunctional(true);
    }
    AggregateInst::ParentSet(this);
    DataNode::parentSet(parent);
}