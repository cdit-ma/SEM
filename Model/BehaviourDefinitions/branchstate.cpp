#include "branchstate.h"
#include "condition.h"

BranchState::BranchState():Branch(){
    //Check Branch for connection rules.
}

BranchState::~BranchState(){}

bool BranchState::canAdoptChild(Node *child)
{
    Condition* condition = dynamic_cast<Condition*>(child);
    if(!condition){
        return false;
    }
    return Branch::canAdoptChild(child);
}
