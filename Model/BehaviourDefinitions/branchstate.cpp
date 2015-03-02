#include "branchstate.h"
#include "condition.h"
#include "termination.h"
#include <QDebug>

BranchState::BranchState():Node()
{

}

BranchState::~BranchState()
{
    //Destructor
}

bool BranchState::canConnect(Node* attachableObject)
{
    Termination* terminate = dynamic_cast<Termination*>(attachableObject);
    if(!terminate){
        //TODO: Implement a check for singular terminate only.
#ifdef DEBUG_MODE
        qWarning() << "Branch state can only connect to 1 Terminate Item";
#endif
        return false;
    }
    //Only Termination.
    return Node::canConnect(attachableObject);
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
    return Node::canAdoptChild(child);
}
