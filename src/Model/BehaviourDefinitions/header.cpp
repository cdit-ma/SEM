#include "header.h"

Header::Header():BehaviourNode(NK_HEADER){
    setWorkflowReciever(false);
    setWorkflowProducer(false);
}

bool Header::canAdoptChild(Node *child)
{
    return false;
}

bool Header::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}
