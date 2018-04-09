#include "condition.h"


Condition::Condition(EntityFactory* factory, NODE_KIND kind, QString kind_str) : ContainerNode(factory, kind, kind_str){
};

Condition::Condition(EntityFactory* factory) : ContainerNode(factory, NODE_KIND::CONDITION, "Condition"){
	auto node_kind = NODE_KIND::CONDITION;
	QString kind_string = "Condition";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Condition();});


    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "value", QVariant::String);
    RegisterDefaultData(factory, node_kind, "row_subgroup", QVariant::Int, true, -1);
};

Condition::Condition(NODE_KIND kind) : ContainerNode(kind){
    setNodeType(NODE_TYPE::CONDITION);
}

bool Condition::canAdoptChild(Node* child)
{
    return ContainerNode::canAdoptChild(child);
}

bool Condition::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return ContainerNode::canAcceptEdge(edgeKind, dst);
}



