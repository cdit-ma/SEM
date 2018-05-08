#include "memberinstance.h"
#include "aggregateinstance.h"
#include "../../entityfactory.h"
#include "../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::MEMBER_INSTANCE;
const QString kind_string = "MemberInstance";

void MemberInstance::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MemberInstance(factory, is_temp_node);
    });
}

MemberInstance::MemberInstance(EntityFactory& factory, bool is_temp) : DataNode(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    addInstancesDefinitionKind(NODE_KIND::MEMBER);
    setChainableDefinition();

    //Setup Data
    factory.AttachData(this, "index", QVariant::Int, -1, true);
    factory.AttachData(this, "type", QVariant::String, "", true);
}


void MemberInstance::parentSet(Node* parent){
    AggregateInstance::ParentSet(this);
    DataNode::parentSet(parent);
}