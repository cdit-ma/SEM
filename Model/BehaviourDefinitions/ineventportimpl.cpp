#include "ineventportimpl.h"
#include <QDebug>
#include "../InterfaceDefinitions/ineventport.h"
#include "../DeploymentDefinitions/member.h"


InEventPortImpl::InEventPortImpl(QString name): Node(Node::NT_IMPL)
{
}

InEventPortImpl::~InEventPortImpl()
{
}


bool InEventPortImpl::canAdoptChild(Node *child)
{
    Member* member = dynamic_cast<Member*> (child);

    if(member == 0){
        qCritical() << "Cannot connect to anything which isn't a Member.";
        return false;
    }

    if(this->isAncestorOf(child) || this->isDescendantOf(child)){
        qCritical() << "Already related to this node.";
        return false;
    }

    return true;
}

bool InEventPortImpl::canConnect(Node* attachableObject)
{
    return true;
}

QString InEventPortImpl::toString()
{
    return QString("InEventPortImpl[%1]: "+this->getName()).arg(this->getID());
}
