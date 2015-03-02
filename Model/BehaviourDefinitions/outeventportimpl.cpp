#include "outeventportimpl.h"
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
    }else{
        if(aParent){
            if(!aParent->isAncestorOf(attachableObject)){
                #ifdef DEBUG_MODE
                qWarning() << "Can only connect to items owned by Component Impl!";
                #endif
                return false;
            }
        }
    }



    return Node::canConnect(attachableObject);
}
