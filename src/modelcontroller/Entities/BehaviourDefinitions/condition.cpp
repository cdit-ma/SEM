#include "condition.h"


Condition::Condition(EntityFactory* factory, NODE_KIND kind, QString kind_str) : Node(factory, kind, kind_str){
};

Condition::Condition(EntityFactory* factory) : Node(factory, NODE_KIND::CONDITION, "Condition"){
	auto node_kind = NODE_KIND::CONDITION;
	QString kind_string = "Condition";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Condition();});


    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "value", QVariant::String);
};

Condition::Condition(NODE_KIND kind) : Node(kind){
    setNodeType(NODE_TYPE::CONDITION);
}

bool Condition::canAdoptChild(Node* child)
{
    if(ContainerNode::canAdoptChild(child)){
        return Node::canAdoptChild(child);
    }
    return false;
}

bool Condition::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}



