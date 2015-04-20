#include "periodicevent.h"
#include "workload.h"
#include "branchstate.h"
#include "outeventportimpl.h"
#include "termination.h"
#include <QDebug>
PeriodicEvent::PeriodicEvent():Node()
{

}

PeriodicEvent::~PeriodicEvent()
{
    //Destructor
}

bool PeriodicEvent::canConnect(Node* attachableObject)
{
    //Limit connections to the parent (ie ComponentImpl) children
    Node* parentNode = getParentNode();
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

    //Only in things locally contained.
    return Node::canConnect(attachableObject);
}

bool PeriodicEvent::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}
