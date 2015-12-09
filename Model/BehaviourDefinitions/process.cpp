#include "process.h"
#include "parameter.h"

Process::Process():BehaviourNode(false, false, false){
    setUnconnectable(false);
}

Process::~Process(){
}

bool Process::canAdoptChild(Node* node)
{
    Parameter* parameter = dynamic_cast<Parameter*>(node);

    if(!parameter){
        return false;
    }
    return BehaviourNode::canAdoptChild(node);
}
