#include "workerdefinitions.h"
#include "BehaviourDefinitions/workload.h"

WorkerDefinitions::WorkerDefinitions(): Node(NK_WORKER_DEFINITIONS)
{
    setTop(1);
}

VIEW_ASPECT WorkerDefinitions::getViewAspect()
{
    return VA_WORKERS;
}

bool WorkerDefinitions::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NK_WORKLOAD:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool WorkerDefinitions::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
