#include "outeventportimpl.h"
#include <QDebug>
#include "../InterfaceDefinitions/outeventport.h"
#include "../DeploymentDefinitions/member.h"


OutEventPortImpl::OutEventPortImpl(QString name): Node(Node::NT_IMPL)
{
    //qDebug() << "Constructed InEventPort: "<< this->getName();
}

OutEventPortImpl::~OutEventPortImpl()
{

}


bool OutEventPortImpl::canAdoptChild(Node *child)
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

bool OutEventPortImpl::canConnect(Node* attachableObject)
{
    return true;
}

QString OutEventPortImpl::toString()
{
    return QString("OutEventPortImpl[%1]: "+this->getName()).arg(this->getID());
}
