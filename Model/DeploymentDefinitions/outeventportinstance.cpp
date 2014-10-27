#include "outeventportinstance.h"

#include "member.h"
#include "../node.h"
#include "ineventportinstance.h"
#include <QDebug>

OutEventPortInstance::OutEventPortInstance(QString name):Node(Node::NT_INSTANCE)
{
    //qDebug() << "Constructed OutEventPortIDL: "<< this->getName();
}

OutEventPortInstance::~OutEventPortInstance()
{

}


bool OutEventPortInstance::canAdoptChild(Node *child)
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

bool OutEventPortInstance::canConnect(Node* attachableObject)
{
    InEventPortInstance* inputEventPort = dynamic_cast<InEventPortInstance*> (attachableObject);

    OutEventPort* outEventPort = dynamic_cast<OutEventPort*>(attachableObject);

    if(inputEventPort == 0 && outEventPort == 0){
        qCritical() << "Cannot connect to anything which isn't a Output Event Port or InEventPort instance";
        return false;
    }

    if(inputEventPort){
        if(inputEventPort->getDataValue("type") != this->getDataValue("type")){
            qCritical() << "Cannot connect 2 Different IDL Types!";
            return false;
        }

        if(inputEventPort->getParentNode() == this->getParentNode()){
            //qCritical() << "Cannot connect 2 Ports from the same component!";
            //return false;
        }
    }

    return true;

}

QString OutEventPortInstance::toString()
{
    return QString("OutEventPortInstance[%1]: "+this->getName()).arg(this->getID());
}
