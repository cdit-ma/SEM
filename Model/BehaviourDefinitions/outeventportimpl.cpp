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
        qWarning() << "InEventPortImpl can only adopt one AggregateInstance";
        return false;
    }

    return Node::canAdoptChild(child);
}

bool OutEventPortImpl::canConnect(Node* attachableObject)
{
    OutEventPort* oep = dynamic_cast<OutEventPort*>(attachableObject);
    Node* aParent = getParentNode();



    if(getDefinition() || !oep){
        qCritical() << "Can Only connect to one OutEventPort";
        return false;
    }

    //Check for connections.
    if(aParent){
        Node* parentDef = aParent->getDefinition();
        if(parentDef){
            if(!parentDef->isAncestorOf(attachableObject)){
                qCritical() << "Can Only connect to an OutEventPort owned by the definition of this ComponentImpl.";
                return false;
            }
        }
    }

    return Node::canConnect(attachableObject);
}
