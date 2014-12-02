#include "outeventport.h"
#include "../BehaviourDefinitions/outeventportimpl.h"
#include "../DeploymentDefinitions/outeventportinstance.h"
#include "aggregate.h"
#include <QDebug>

OutEventPort::OutEventPort(QString name):EventPort()
{
}

OutEventPort::~OutEventPort()
{

}


QString OutEventPort::toString()
{
    return QString("OutEventPort[%1]: "+this->getName()).arg(this->getID());

}

bool OutEventPort::canConnect(Node* attachableObject)
{
    OutEventPortImpl* outEventPortImpl = dynamic_cast<OutEventPortImpl*>(attachableObject);
    OutEventPortInstance* outEventPortInstance = dynamic_cast<OutEventPortInstance*>(attachableObject);
    Aggregate* aggregate = dynamic_cast<Aggregate*>(attachableObject);


    if(!aggregate && !outEventPortImpl && !outEventPortInstance){
        qWarning() << "A OutEventPort can only connect to a Aggregate, OutEventPortImpl or OutEventPortInstance.";
        return false;
    }

    return EventPort::canConnect(attachableObject);
}

bool OutEventPort::canAdoptChild(Node *child)
{
    return EventPort::canAdoptChild(child);
}

