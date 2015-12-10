#include "periodicevent.h"
#include "inputparameter.h"

PeriodicEvent::PeriodicEvent():BehaviourNode(){
    //Setup initial settings
    setIsWorkflowStart(true);
}

PeriodicEvent::~PeriodicEvent(){
}

bool PeriodicEvent::canAdoptChild(Node * node)
{
    InputParameter* inputParameter = dynamic_cast<InputParameter*>(node);

    if(!inputParameter){
        return false;
    }

    return BehaviourNode::canAdoptChild(node);
}
