#include "setter.h"

#include "../entityfactory.h"

Setter::Setter(EntityFactory* factory) : BehaviourNode(factory, NODE_KIND::SETTER, "Setter"){
	auto node_kind = NODE_KIND::SETTER;
	QString kind_string = "Setter";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Setter();});


    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "operator", QVariant::String, false, "=");
};

Setter::Setter():BehaviourNode(NODE_KIND::SETTER)
{
    setWorkflowProducer(true);
    setWorkflowReciever(true);
}

bool Setter::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::INPUT_PARAMETER:
    case NODE_KIND::VARIADIC_PARAMETER:
        break;
    default:
        return false;
    }

    if(!getChildrenOfKind(child->getNodeKind(), 0).isEmpty()){
        return false;
    }

    return BehaviourNode::canAdoptChild(child);
}

bool Setter::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}
