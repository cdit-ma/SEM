#include "InEventPortIDL.h"
#include <QDebug>
#include <typeinfo>
#include "outeventportidl.h"

InEventPortIDL::InEventPortIDL(QString name):EventPort(name)
{
    qDebug() << "Constructed InEventPortIDL: "<< this->getName();
}

InEventPortIDL::~InEventPortIDL()
{

}

bool InEventPortIDL::isAdoptLegal(GraphMLContainer *child)
{
    Q_UNUSED(child);
    return true;
}

bool InEventPortIDL::isEdgeLegal(GraphMLContainer *attachableObject)
{
    OutEventPortIDL* outputEventPort = dynamic_cast<OutEventPortIDL*> (attachableObject);

    if(outputEventPort == 0){
        qCritical() << "Cannot connect to anything which isn't a Output EVent Port";
        return false;
    }

    if(outputEventPort->getDataValue("type") != this->getDataValue("type")){
        qCritical() << "Cannot connect 2 Different IDL Types!";
        return false;
    }

    if(outputEventPort->getParent() == this->getParent()){
         //qCritical() << "Cannot connect 2 Ports from the same component!";
         //return false;
    }


    if(this->isConnected(attachableObject)){
        qCritical() << "Cannot connect 2 already connected ports!";
        return false;
    }

    return true;
}


QString InEventPortIDL::toString()
{
    return QString("InEventPortIDL[%1]: "+this->getName()).arg(this->getID());

}
