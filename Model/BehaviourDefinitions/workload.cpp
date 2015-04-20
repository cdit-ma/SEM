#include "workload.h"
#include "branchstate.h"
#include "outeventportimpl.h"
#include "termination.h"
#include <QDebug>
#include "process.h"
Workload::Workload():Node()
{
    //qWarning() << "Constructed Workload: "<< this->getName();

}

Workload::~Workload()
{
    //Destructor
}

bool Workload::canConnect(Node* attachableObject)
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
    return Node::canConnect(attachableObject);
}

bool Workload::canAdoptChild(Node *child)
{
    Process* process = dynamic_cast<Process*>(child);
    if(!process){
#ifdef DEBUG_MODE
         qWarning() << "Workload can only adopt Process!";
#endif
        return false;
    }
    return Node::canAdoptChild(child);
}
