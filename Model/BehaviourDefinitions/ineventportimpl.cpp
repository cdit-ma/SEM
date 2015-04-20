#include "ineventportimpl.h"
#include "workload.h"
#include "branchstate.h"
#include "outeventportimpl.h"
#include "termination.h"
#include <QDebug>
#include "../InterfaceDefinitions/ineventport.h"
#include "../InterfaceDefinitions/member.h"
#include "../InterfaceDefinitions/aggregateinstance.h"

InEventPortImpl::InEventPortImpl(QString name): Node(Node::NT_IMPL)
{
}

InEventPortImpl::~InEventPortImpl()
{
}


bool InEventPortImpl::canAdoptChild(Node *child)
{
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);

    if(!aggregateInstance || (aggregateInstance && this->childrenCount() > 0)){
        #ifdef DEBUG_MODE
        qWarning() << "InEventPortImpl can only adopt one AggregateInstance";
        #endif
        return false;
    }

    return Node::canAdoptChild(child);
}

bool InEventPortImpl::canConnect(Node* attachableObject)
{

    InEventPort* oep = dynamic_cast<InEventPort*>(attachableObject);

    Node* aParent = getParentNode();

    if(oep){
        if(getDefinition() && oep ){
#ifdef DEBUG_MODE
            qWarning() << "Can Only connect to one InEventPort";
#endif
            return false;
        }
        if(aParent){
            Node* parentDef = aParent->getDefinition();
            if(parentDef){
                if(!parentDef->isAncestorOf(attachableObject)){
#ifdef DEBUG_MODE
                    qWarning() << "Can Only connect to an InEventPort owned by the definition of this ComponentImpl.";
#endif
                    return false;
                }
            }
        }

    }else{ // not definition connection, must be behaviour connection

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
    }
    return Node::canConnect(attachableObject);
}
