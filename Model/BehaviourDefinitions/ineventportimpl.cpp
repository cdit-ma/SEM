#include "ineventportimpl.h"
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
        qWarning() << "InEventPortImpl can only adopt one AggregateInstance";
        return false;
    }

    return Node::canAdoptChild(child);
}

bool InEventPortImpl::canConnect(Node* attachableObject)
{

    InEventPort* oep = dynamic_cast<InEventPort*>(attachableObject);

    if(getDefinition() && oep ){
        qCritical() << "Can Only connect to one InEventPort";
        return false;
    }

    return Node::canConnect(attachableObject);
}
