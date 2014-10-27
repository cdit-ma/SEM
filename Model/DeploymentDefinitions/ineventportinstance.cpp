#include "ineventportinstance.h"
#include <QDebug>
#include <typeinfo>
#include "outeventportinstance.h"
#include "../InterfaceDefinitions/ineventport.h"
#include "member.h"

InEventPortInstance::InEventPortInstance(QString name):Node(Node::NT_INSTANCE)
{
    //qDebug() << "Constructed InEventPortIDL: "<< this->getName();
}

InEventPortInstance::~InEventPortInstance()
{
   // foreach(InEventPort* child, inEventPorts){
    //    delete child;
   // }

}


bool InEventPortInstance::canAdoptChild(Node *child)
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

bool InEventPortInstance::canConnect(Node* attachableObject)
{
    OutEventPortInstance* outputEventPort = dynamic_cast<OutEventPortInstance*> (attachableObject);

    InEventPort* inEventPort = dynamic_cast<InEventPort*>(attachableObject);

    if(outputEventPort == 0 && inEventPort == 0){
        qCritical() << "Cannot connect to anything which isn't a Output Event Port or InEventPort instance";
        return false;
    }

    if(outputEventPort){
        if(outputEventPort->getDataValue("type") != this->getDataValue("type")){
            qCritical() << "Cannot connect 2 Different IDL Types!";
            return false;
        }

        if(outputEventPort->getParentNode() == this->getParentNode()){
            //qCritical() << "Cannot connect 2 Ports from the same component!";
            //return false;
        }
    }

    return true;
}


QString InEventPortInstance::toString()
{
    return QString("InEventPortInstance[%1]: "+this->getName()).arg(this->getID());

}


