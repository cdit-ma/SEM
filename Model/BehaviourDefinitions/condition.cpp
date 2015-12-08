#include "condition.h"
#include "termination.h"
#include "branch.h"

Condition::Condition():BehaviourNode(true, false, false){
    addValidEdgeType(Edge::EC_WORKFLOW);
}

Condition::~Condition(){}

Branch *Condition::getBranch()
{
    Branch* parent = dynamic_cast<Branch*>(getParentNode());
    return parent;
}

Termination *Condition::getTermination()
{
    Branch* branch = getBranch();
    if(branch){
        Termination* termination = branch->getTermination();
        if(termination){
            if(termination->isConnected(this)){
                return termination;
            }
        }
    }
    return 0;
}

bool Condition::canAdoptChild(Node*)
{
    return false;
}

bool Condition::canConnect_WorkflowEdge(Node *node)
{
    Termination* termination = dynamic_cast<Termination*>(node);

    Branch* parentBranch = getBranch();

    if(parentBranch){
        if(termination){
            if(!parentBranch->getTermination()){
                //If a Conditions Parent Branch Doesn't have a Termination attached, don't allow connection to a Termination yet.
                return false;
            }
        }else{
            //If the node isn't a termination, check to stop indirect chains.
            if(parentBranch->isIndirectlyConnected(node)){
                //Don't allow conditions to connect to chains already connected to.
                return false;
            }
        }
    }
    return BehaviourNode::canConnect_WorkflowEdge(node);
}


