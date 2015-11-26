#include "workerdefinitions.h"
#include "BehaviourDefinitions/workload.h"
#include "QDebug"

WorkerDefinitions::WorkerDefinitions(): Node(Node::NT_ASPECT)
{
    setTop(1);
    qCritical() << getTreeIndex();
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
