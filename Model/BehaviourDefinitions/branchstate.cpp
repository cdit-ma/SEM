#include "branchstate.h"
#include "condition.h"
#include "termination.h"
#include <QDebug>

BranchState::BranchState():Branch(){}

BranchState::~BranchState(){}



Edge::EDGE_CLASS BranchState::canConnect(Node* attachableObject)
{
    Termination* terminate = dynamic_cast<Termination*>(attachableObject);

    //Can Only connect to a Terminate.
    if(!terminate){
        return false;
    }

    return Branch::canConnect(attachableObject);
}

bool BranchState::canAdoptChild(Node *child)
{
    Condition* condition = dynamic_cast<Condition*>(child);
    if(!condition){
        #ifdef DEBUG_MODE
        qWarning() << "Branch state can only adopt Conditions.";
        #endif
        return false;
    }
    return Branch::canAdoptChild(child);
}
