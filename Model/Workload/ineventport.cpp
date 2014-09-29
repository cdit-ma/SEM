#include "ineventport.h"
#include "../Assembly/ineventportidl.h"
#include <QDebug>
#include "../Assembly/member.h"
InEventPort::InEventPort(QString name): Node(name)
{
    qDebug() << "Constructed InEventPort: "<< this->getName();
    inEventPortIDL = 0;
}

InEventPort::~InEventPort()
{
    if(inEventPortIDL != 0){
        inEventPortIDL->removeInEventPortInstance(this);
    }

}

void InEventPort::setInEventPortIDL(InEventPortIDL *parent)
{
    inEventPortIDL = parent;

}

InEventPortIDL *InEventPort::getInEventPortIDL()
{
    return inEventPortIDL;
}

bool InEventPort::isAdoptLegal(GraphMLContainer *child)
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

bool InEventPort::isEdgeLegal(GraphMLContainer *attachableObject)
{
    return true;
}

QString InEventPort::toString()
{
    return QString("InEventPort[%1]: "+this->getName()).arg(this->getID());
}
