#include "member.h"
#include "aggregateinstance.h"

Member::Member(EntityFactory* factory) : DataNode(factory, NODE_KIND::MEMBER, "Member"){
	auto node_kind = NODE_KIND::MEMBER;
	QString kind_string = "Member";
    

    //Allow reordering
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Member();});

    QList<QVariant> types = {"String",  "Boolean", "Integer", "Double", "Float", "Character"};


    //Register values
    RegisterValidDataValues(factory, node_kind, "type", QVariant::String, types);
    RegisterValidDataValues(factory, NODE_KIND::ATTRIBUTE, "type", QVariant::String, types);
    RegisterValidDataValues(factory, NODE_KIND::DEPLOYMENT_ATTRIBUTE, "type", QVariant::String, types);

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, false, "String");
    RegisterDefaultData(factory, node_kind, "key", QVariant::Bool, false, false);
};

Member::Member(): DataNode(NODE_KIND::MEMBER)
{
    addInstanceKind(NODE_KIND::MEMBER_INSTANCE);
}

void Member::parentSet(Node* parent){
    AggregateInstance::ParentSet(this);
    DataNode::parentSet(parent);
}
