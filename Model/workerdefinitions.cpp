#include "workerdefinitions.h"
#include "BehaviourDefinitions/workload.h"

WorkerDefinitions::WorkerDefinitions(): Node(Node::NT_ASPECT)
{
    setTop(1);
}

bool WorkerDefinitions::canAdoptChild(Node *node)
{
    Workload* workload = dynamic_cast<Workload*>(node);
    if(!workload){
        return false;
    }
    return Node::canAdoptChild(node);
}
