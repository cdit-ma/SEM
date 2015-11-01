#include "periodicevent.h"
#include "workload.h"
#include "branchstate.h"
#include "outeventportimpl.h"
#include "termination.h"
#include "inputparameter.h"
#include "parameter.h"
#include <QDebug>
PeriodicEvent::PeriodicEvent():BehaviourNode(true){}

PeriodicEvent::~PeriodicEvent(){}

bool PeriodicEvent::canConnect(Node* attachableObject)
{
    return BehaviourNode::canConnect(attachableObject);
}

bool PeriodicEvent::canAdoptChild(Node * node)
{
    //ONLY ALLOW
    Parameter* p = dynamic_cast<Parameter*>(node);
    if(!p){
        return false;
    }

    return BehaviourNode::canAdoptChild(p);
}
