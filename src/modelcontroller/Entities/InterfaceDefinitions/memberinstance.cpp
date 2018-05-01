#include "memberinstance.h"
#include "aggregateinstance.h"

MemberInstance::MemberInstance(EntityFactory* factory) : DataNode(factory, NODE_KIND::MEMBER_INSTANCE, "MemberInstance"){
	auto node_kind = NODE_KIND::MEMBER_INSTANCE;
	QString kind_string = "MemberInstance";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new MemberInstance();});

    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "value", QVariant::String, false);
};

MemberInstance::MemberInstance():DataNode(NODE_KIND::MEMBER_INSTANCE)
{
    addInstancesDefinitionKind(NODE_KIND::MEMBER);
    setChainableDefinition();
}


bool MemberInstance::canAdoptChild(Node*)
{
    return false;
}

void MemberInstance::parentSet(Node* parent){
    AggregateInstance::ParentSet(this);
    DataNode::parentSet(parent);
}
