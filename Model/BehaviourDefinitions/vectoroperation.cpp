#include "vectoroperation.h"
#include "parameter.h"

VectorOperation::VectorOperation():BehaviourNode(false, false, false){
    setUnconnectable(false);
}

VectorOperation::~VectorOperation(){}

bool VectorOperation::canConnect(Node*)
{
    return false;
}

bool VectorOperation::canAdoptChild(Node* node)
{
    Parameter* p = dynamic_cast<Parameter*>(node);
    if(!p){
        return false;
    }
    return BehaviourNode::canAdoptChild(p);
}

