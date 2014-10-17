#include "ineventportimpl.h"
#include <QDebug>
#include "../InterfaceDefinitions/ineventport.h"
#include "../DeploymentDefinitions/member.h"


InEventPortImpl::InEventPortImpl(QString name): Node(name)
{
    //qDebug() << "Constructed InEventPort: "<< this->getName();
    def = 0;
}

InEventPortImpl::~InEventPortImpl()
{
    if(def){
        def->setImpl(0);
    }

}

void InEventPortImpl::setDefinition(InEventPort *parent)
{
    def = parent;

}

InEventPort *InEventPortImpl::getDefinition()
{
    return def;
}

bool InEventPortImpl::isAdoptLegal(GraphMLContainer *child)
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

bool InEventPortImpl::isEdgeLegal(GraphMLContainer *attachableObject)
{
    return true;
}

QString InEventPortImpl::toString()
{
    return QString("InEventPortImpl[%1]: "+this->getName()).arg(this->getID());
}
