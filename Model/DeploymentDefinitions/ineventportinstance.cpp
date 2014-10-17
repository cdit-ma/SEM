#include "ineventportinstance.h"
#include <QDebug>
#include <typeinfo>
#include "outeventportinstance.h"
#include "../InterfaceDefinitions/ineventport.h"
#include "member.h"

InEventPortInstance::InEventPortInstance(QString name):EventPort(name)
{
    //qDebug() << "Constructed InEventPortIDL: "<< this->getName();
    def = 0;
}

InEventPortInstance::~InEventPortInstance()
{
   // foreach(InEventPort* child, inEventPorts){
    //    delete child;
   // }

}

void InEventPortInstance::setDefinition(InEventPort *def)
{
    this->def  = def;

}

InEventPort *InEventPortInstance::getDefinition()
{
    return def;
}

InEventPortImpl *InEventPortInstance::getImpl()
{
    return def->getImpl();
}

bool InEventPortInstance::isAdoptLegal(GraphMLContainer *child)
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

bool InEventPortInstance::isEdgeLegal(GraphMLContainer *attachableObject)
{
    /*
    OutEventPortInstance* outputEventPort = dynamic_cast<OutEventPortInstance*> (attachableObject);

    InEventPortImpl* inEventPort = dynamic_cast<InEventPortImpl*>(attachableObject);

    if(outputEventPort == 0 && inEventPort == 0){
        qCritical() << "Cannot connect to anything which isn't a Output Event Port or InEventPort instance";
        return false;
    }

    if(outputEventPort){
        if(outputEventPort->getDataValue("type") != this->getDataValue("type")){
            qCritical() << "Cannot connect 2 Different IDL Types!";
            return false;
        }

        if(outputEventPort->getParent() == this->getParent()){
            //qCritical() << "Cannot connect 2 Ports from the same component!";
            //return false;
        }
    }

    if(inEventPort){

    }

    if(this->isConnected(attachableObject)){
        qCritical() << "Cannot connect 2 already connected ports!";
        return false;
    }

    */
    return true;
}


QString InEventPortInstance::toString()
{
    return QString("InEventPortInstance[%1]: "+this->getName()).arg(this->getID());

}


