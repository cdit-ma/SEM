#include "workload.h"

Workload::Workload():BehaviourNode(NK_WORKLOAD){
    setWorkflowReciever(true);
    setWorkflowProducer(true);

    setExpandEnabled(true);
}

bool Workload::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NK_PROCESS:
    case NK_WORKER_PROCESS:
        break;
    default:
        return false;
    }
    return BehaviourNode::canAdoptChild(child);
}

bool Workload::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}
