#include "condition.h"
#include "workload.h"
#include "branchstate.h"
#include "outeventportimpl.h"
#include "termination.h"
#include <QDebug>
Condition::Condition():Node()
{
    //qWarning() << "Constructed Condition: "<< this->getName();
}

Condition::~Condition()
{
    //Destructor
}

bool Condition::canConnect(Node* attachableObject)
{
//    return Node::canConnect(attachableObject);

    //Limit connections to the grandparent (ie BranchState->ComponentImpl) children
    Node* parentNode = getParentNode()->getParentNode();
    if(parentNode){
        if(!parentNode->isAncestorOf(attachableObject)){
            return false;
        }
    }

    // Limit connections in behavior to Workload BranchState OutEventPortImpl and Termination.
    Workload* workload = dynamic_cast<Workload*>(attachableObject);
    BranchState* branchstate = dynamic_cast<BranchState*>(attachableObject);
    OutEventPortImpl* outeventportimpl = dynamic_cast<OutEventPortImpl*>(attachableObject);
    Termination* terminate = dynamic_cast<Termination*>(attachableObject);

    if (!workload && !branchstate && !outeventportimpl && !terminate){
        return false;
    }
    return Node::canConnect(attachableObject);
}

bool Condition::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}
