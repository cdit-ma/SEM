#include "workerdefinitions.h"
#include "BehaviourDefinitions/workload.h"

WorkerDefinitions::WorkerDefinitions(): Node(Node::NT_ASPECT, NC_WORKER_DEFINITIONS)
{
    setTop(1);
}

VIEW_ASPECT WorkerDefinitions::getViewAspect()
{
    return VA_WORKERS;
}

bool WorkerDefinitions::canAdoptChild(Node *node)
{
    Workload* workload = dynamic_cast<Workload*>(node);
    if(!workload){
        return false;
    }
    return Node::canAdoptChild(node);
}
