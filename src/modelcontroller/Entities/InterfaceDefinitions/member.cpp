#include "member.h"
#include "aggregateinstance.h"
#include "../../entityfactory.h"
#include "../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::MEMBER;
const QString kind_string = "Member";

void Member::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new Member(factory, is_temp_node);
    });
}

Member::Member(EntityFactory& factory, bool is_temp) : DataNode(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    addInstanceKind(NODE_KIND::MEMBER_INSTANCE);

    //Setup Data
    factory.AttachData(this, "index", QVariant::Int, -1, false);
    auto type_data = factory.AttachData(this, "type", QVariant::String, "String", false);
    type_data->addValidValues(TypeKey::GetValidPrimitiveTypes());
    factory.AttachData(this, "key", QVariant::Bool, false, false);
}


void Member::parentSet(Node* parent){
    AggregateInstance::ParentSet(this);
    DataNode::parentSet(parent);
}
