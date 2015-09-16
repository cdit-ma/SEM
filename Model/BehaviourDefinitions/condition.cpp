#include "condition.h"
#include "workload.h"
#include "branchstate.h"
#include "outeventportimpl.h"
#include "termination.h"
#include "branch.h"
#include "../node.h"
#include <QDebug>
Condition::Condition():BehaviourNode(true, false, false){}

Condition::~Condition(){}

Branch *Condition::getBranch()
{
    Branch* parent = dynamic_cast<Branch*>(getParentNode());
    return parent;
}

Termination *Condition::getTermination()
{

    if(getBranch()){
        Termination* t = getBranch()->getTermination();
        if(t->isConnected(this)){
            return t;
        }
    }
    return 0;
}


bool Condition::canConnect(Node* attachableObject)
{
    //Get Parent
    if(attachableObject->getNodeKind() == "Termination"){
        Branch* parentBranch = getBranch();
        //If a Conditions Parent Branch Doesn't have a Termination attached, don't allow it.
        if(parentBranch){
            if(!parentBranch->getTermination()){
                return false;
            }
        }
    }else{
         Branch* parentBranch = getBranch();
        if(parentBranch->isIndirectlyConnected(attachableObject)){
            return false;
        }
    }

    return BehaviourNode::canConnect(attachableObject);
}

bool Condition::canAdoptChild(Node*)
{
    return false;
}
