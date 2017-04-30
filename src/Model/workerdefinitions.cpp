#include "workerdefinitions.h"
#include "BehaviourDefinitions/workload.h"

WorkerDefinitions::WorkerDefinitions(): Node(NODE_KIND::WORKER_DEFINITIONS)
{
    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "WORKERS");
}


VIEW_ASPECT WorkerDefinitions::getViewAspect() const
{
    return VA_WORKERS;
}

bool WorkerDefinitions::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::WORKLOAD:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool WorkerDefinitions::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
