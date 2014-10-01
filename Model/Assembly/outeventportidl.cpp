#include "outeventportidl.h"
#include "ineventportidl.h"
#include "member.h"
#include <QDebug>

OutEventPortIDL::OutEventPortIDL(QString name):EventPort(name)
{
    //qDebug() << "Constructed OutEventPortIDL: "<< this->getName();
}

OutEventPortIDL::~OutEventPortIDL()
{

}

bool OutEventPortIDL::isAdoptLegal(GraphMLContainer *child)
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

bool OutEventPortIDL::isEdgeLegal(GraphMLContainer *attachableObject)
{
    InEventPortIDL* inputEventPort = dynamic_cast<InEventPortIDL*> (attachableObject);

    //Check Event Name (TYPE)
    if(inputEventPort == 0){
        qCritical() << "Cannot connect to anything which isn't a Input Event Port";
        return false;
    }

    if(inputEventPort->getDataValue("type") != this->getDataValue("type")){
        qCritical() << "Cannot connect 2 Different IDL Types!";
        return false;
    }

    if(inputEventPort->getParent() == this->getParent()){
         qCritical() << "Cannot connect 2 Ports from the same component!";
         //return false;
    }

    if(this->isConnected(attachableObject)){
        qCritical() << "Cannot connect 2 already connected ports!";
        return false;
    }

    return true;

}

QString OutEventPortIDL::toString()
{
    return QString("OutEventPortIDL[%1]: "+this->getName()).arg(this->getID());
}
