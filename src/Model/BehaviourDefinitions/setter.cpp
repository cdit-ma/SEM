#include "setter.h"

Setter::Setter():BehaviourNode(NODE_KIND::SETTER)
{
    setWorkflowProducer(true);
    setWorkflowReciever(true);

    updateDefaultData("operator", QVariant::String, false, "=");
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

bool Setter::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}
