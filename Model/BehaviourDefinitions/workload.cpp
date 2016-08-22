#include "workload.h"

Workload::Workload():BehaviourNode(NK_WORKLOAD){
}

bool Workload::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NK_PROCESS:
        break;
    default:
        return false;
    }
    return BehaviourNode::canAdoptChild(child);
}

bool Workload::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}
