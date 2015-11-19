#include "workerdefinitions.h"
#include "BehaviourDefinitions/workload.h"

WorkerDefinitions::WorkerDefinitions(): Node(Node::NT_ASPECT)
{
}

bool WorkerDefinitions::canAdoptChild(Node *node)
{
    //Workloads!
    Workload* workload = dynamic_cast<Workload*>(node);
    if(!workload){
        return false;
    }
    return Node::canAdoptChild(node);
}

bool WorkerDefinitions::canConnect(Node *node)
{
    Q_UNUSED(node);
    return false;
}
