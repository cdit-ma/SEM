#include "workload.h"
#include "branchstate.h"
#include "outeventportimpl.h"
//#include "vectoroperation.h"
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
    /*VectorOperation* vectorOperation = dynamic_cast<VectorOperation*>(child);
    if(!process && !vectorOperation){
#ifdef DEBUG_MODE
         qWarning() << "Workload can only adopt Process!";
#endif
        return false;
    }*/
    return BehaviourNode::canAdoptChild(child);
}
