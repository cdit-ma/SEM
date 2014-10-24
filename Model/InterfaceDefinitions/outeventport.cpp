#include "outeventport.h"
#include "../BehaviourDefinitions/outeventportimpl.h"
#include "../DeploymentDefinitions/outeventportinstance.h"

OutEventPort::OutEventPort(QString name):Node(name,Node::NT_DEFINITION)
{
    impl = 0;
}

OutEventPort::~OutEventPort()
{

}

void OutEventPort::addInstance(OutEventPortInstance *instance)
{
    if(!instances.contains(instance)){
        instances.append(instance);
        instance->setDefinition(this);
    }
}

void OutEventPort::removeInstance(OutEventPortInstance *instance)
{
    int index = instances.indexOf(instance);
    if(index >= 0){
        instance->setDefinition(0);
        instances.removeAt(index);
    }
}

QVector<OutEventPortInstance *> OutEventPort::getInstances()
{
    return instances;
}

void OutEventPort::setImpl(OutEventPortImpl *impl)
{
    this->impl = impl;
}

OutEventPortImpl *OutEventPort::getImpl()
{
    return impl;
}

QString OutEventPort::toString()
{
    return QString("OutEventPort[%1]: "+this->getName()).arg(this->getID());

}

bool OutEventPort::canConnect(Node* attachableObject)
{
    return true;
}

bool OutEventPort::canAdoptChild(Node *child)
{
    return true;
}

