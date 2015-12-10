#include "branch.h"
#include "condition.h"

Branch::Branch():BehaviourNode(){
}

Termination *Branch::getTermination()
{
    BehaviourNode* node = this;

    while(node){
        Termination* termination = dynamic_cast<Termination*>(node);
        if(termination){
            return termination;
        }
        node = node->getRightBehaviourNode();
    }
    return 0;
}

bool Branch::canAdoptChild(Node *node)
{
    Condition* condition = dynamic_cast<Condition*>(node);

    if(!condition){
        return false;
    }

    return BehaviourNode::canAdoptChild(node);
}

bool Branch::canConnect_WorkflowEdge(Node *node)
{
    Termination* terminate = dynamic_cast<Termination*>(node);

    if(terminate){
        if(terminate->getBranch()){
            //Can't connect to a Termination which has a Branch attacted.
            return false;
        }
        if(getTermination()){
            //Can't connect to a Termination, if already have a Termination()
            return false;
        }
    }else{
        if(!getTermination()){
            //If we don't have a termination yet, we can't connect.
            return false;
        }
    }

    return BehaviourNode::canConnect_WorkflowEdge(node);
}

