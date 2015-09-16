#include "workload.h"
#include "branchstate.h"
#include "outeventportimpl.h"
#include "termination.h"
#include <QDebug>
#include "process.h"

Workload::Workload():BehaviourNode(){}
Workload::~Workload(){}

bool Workload::canConnect(Node* attachableObject)
{
    return BehaviourNode::canConnect(attachableObject);
}

bool Workload::canAdoptChild(Node *child)
{
    Process* process = dynamic_cast<Process*>(child);
    if(!process){
#ifdef DEBUG_MODE
         qWarning() << "Workload can only adopt Process!";
#endif
        return false;
    }
    return BehaviourNode::canAdoptChild(child);
}
