#include "whileloop.h"
#include "branchstate.h"
#include "condition.h"
#include "termination.h"
#include <QDebug>

WhileLoop::WhileLoop():Branch(){}

WhileLoop::~WhileLoop(){}



bool WhileLoop::canConnect(Node* attachableObject)
{
    Termination* terminate = dynamic_cast<Termination*>(attachableObject);

    //Can Only connect to a Terminate.
    if(!terminate){
        return false;
    }

    return Branch::canConnect(attachableObject);
}

bool WhileLoop::canAdoptChild(Node *child)
{
    Condition* condition = dynamic_cast<Condition*>(child);
    if(!condition){
        #ifdef DEBUG_MODE
        qWarning() << "Branch state can only adopt Conditions.";
        #endif
        return false;
    }else{
        if(childrenCount() > 0){
            return false;
        }
    }

    return Branch::canAdoptChild(child);
}

