#include "ineventportimpl.h"
#include <QDebug>
#include "../InterfaceDefinitions/ineventport.h"
#include "../InterfaceDefinitions/member.h"


InEventPortImpl::InEventPortImpl(QString name): Node(Node::NT_IMPL)
{
}

InEventPortImpl::~InEventPortImpl()
{
}


bool InEventPortImpl::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}

bool InEventPortImpl::canConnect(Node* attachableObject)
{
    InEventPort* oep = dynamic_cast<InEventPort*>(attachableObject);

    if(getDefinition() && oep){
        qCritical() << "Can Only connect an InEventPortImpl to an InEventPort";
        return false;
    }

    return Node::canConnect(attachableObject);
}

QString InEventPortImpl::toString()
{
    return QString("InEventPortImpl[%1]: "+this->getName()).arg(this->getID());
}
