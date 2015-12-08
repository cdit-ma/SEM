#include "workload.h"
#include "branchstate.h"
#include "outeventportimpl.h"
#include "termination.h"
#include <QDebug>
#include "process.h"
#include "variable.h"
#include "parameter.h"

Workload::Workload():BehaviourNode(){}
Workload::~Workload(){}

Edge::EDGE_CLASS Workload::canConnect(Node* attachableObject)
{


    return BehaviourNode::canConnect(attachableObject);
}

bool Workload::canAdoptChild(Node *child)
{
    Process* process = dynamic_cast<Process*>(child);
    Variable* variable = dynamic_cast<Variable*>(child);
    Parameter* parameter = dynamic_cast<Parameter*>(child);
    if(!(process || variable || parameter)){
#ifdef DEBUG_MODE
         qWarning() << "Workload can only adopt Process!";
#endif
        return false;
    }
    return BehaviourNode::canAdoptChild(child);
}
