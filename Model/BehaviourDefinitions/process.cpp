#include "process.h"
#include "parameter.h"

Process::Process():BehaviourNode(){
    setIsNonWorkflow(true);
}

Process::~Process(){
}

bool Process::canAdoptChild(Node* node)
{
    Parameter* parameter = dynamic_cast<Parameter*>(node);

    if(!parameter){
        return false;
    }

    foreach(Node* c, getChildren(0)){
        Parameter* child = dynamic_cast<Parameter*>(c);
        if(child->isReturnParameter() && parameter->isReturnParameter()){
            return false;
        }
    }

    return BehaviourNode::canAdoptChild(node);
}
