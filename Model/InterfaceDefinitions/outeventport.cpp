#include "outeventport.h"
#include "../BehaviourDefinitions/outeventportimpl.h"
#include "../DeploymentDefinitions/outeventportinstance.h"

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
    return EventPort::canConnect(attachableObject);
}

bool OutEventPort::canAdoptChild(Node *child)
{
    return EventPort::canAdoptChild(child);
}

