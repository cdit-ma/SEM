#include "setter.h"

Setter::Setter():BehaviourNode(NK_SETTER)
{
}

bool Setter::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
    case NK_INPUT_PARAMETER:
        break;
    default:
        return false;
    }
    return BehaviourNode::canAdoptChild(child);
}

bool Setter::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}
