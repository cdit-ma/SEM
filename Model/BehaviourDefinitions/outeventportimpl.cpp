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

    if(getDefinition() && oep ){
        qCritical() << "Can Only connect to one OutEventPort";
        return false;
    }

    return Node::canConnect(attachableObject);
}

QString OutEventPortImpl::toString()
{
    return QString("OutEventPortImpl[%1]: "+this->getName()).arg(this->getID());
}
