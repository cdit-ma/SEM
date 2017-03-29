#include "setter.h"

Setter::Setter():BehaviourNode(NK_SETTER)
{
    setWorkflowProducer(true);
    setWorkflowReciever(true);
}

bool Setter::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
    case NK_INPUT_PARAMETER:
    case NK_VARIADIC_PARAMETER:
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
