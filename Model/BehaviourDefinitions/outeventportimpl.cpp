#include "outeventportimpl.h"
#include "workload.h"
#include "branchstate.h"
#include "outeventportimpl.h"
#include "termination.h"
#include <QDebug>
#include "../InterfaceDefinitions/outeventport.h"
#include "../InterfaceDefinitions/member.h"
#include "../InterfaceDefinitions/aggregateinstance.h"

OutEventPortImpl::OutEventPortImpl(QString name): Node(Node::NT_IMPL)
{
    //qDebug() << "Constructed InEventPort: "<< this->getName();
}

OutEventPortImpl::~OutEventPortImpl()
{

}


bool OutEventPortImpl::canAdoptChild(Node *child)
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

bool OutEventPortImpl::canConnect(Node* attachableObject)
{
    OutEventPort* oep = dynamic_cast<OutEventPort*>(attachableObject);

    Node* aParent = getParentNode();

    if(oep){
        if(getDefinition()){
#ifdef DEBUG_MODE
            qWarning() << "OutEventPortImpl Can Only be connected to one OutEventPort Definition.";
#endif
            return false;
        }
        if(aParent){
            Node* parentDef = aParent->getDefinition();
            if(parentDef){
                if(!parentDef->isAncestorOf(attachableObject)){
#ifdef DEBUG_MODE
                    qWarning() << "Can Only connect to an OutEventPort owned by the definition of this ComponentImpl.";
#endif
                    return false;
                }
            }
        }
    }else{ // not definition connection, must be behaviour connection

        //Cannot connect to an undefined thing.
        if(!this->getDefinition()){
            return false;
        }
        //Limit connections to the parent (ie ComponentImpl) children
        if(aParent){
            if(!aParent->isAncestorOf(attachableObject)){
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
