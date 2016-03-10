#include "workload.h"

#include "process.h"
#include "variable.h"
#include "parameter.h"

Workload::Workload():BehaviourNode(){

}

Workload::~Workload(){

}

bool Workload::canAdoptChild(Node *child)
{
    Process* process = dynamic_cast<Process*>(child);
    Variable* variable = dynamic_cast<Variable*>(child);

    if(!process && !variable){
        return false;
    }

    return BehaviourNode::canAdoptChild(child);
}
