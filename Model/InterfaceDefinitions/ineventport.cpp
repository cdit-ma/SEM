#include "ineventport.h"
#include "../BehaviourDefinitions/ineventportimpl.h"
#include "../DeploymentDefinitions/ineventportinstance.h"


InEventPort::InEventPort(QString name):Node(name)
{
    impl = 0;
}

InEventPort::~InEventPort()
{

}

void InEventPort::addInstance(InEventPortInstance *instance)
{
    if(!instances.contains(instance)){
        instances.append(instance);
        instance->setDefinition(this);
    }
}

void InEventPort::removeInstance(InEventPortInstance *instance)
{
    int index = instances.indexOf(instance);
    if(index >= 0){
        instance->setDefinition(0);
        instances.removeAt(index);
    }
}

QVector<InEventPortInstance *> InEventPort::getInstances()
{
    return instances;
}

void InEventPort::setImpl(InEventPortImpl *impl)
{
    this->impl = impl;
}

InEventPortImpl *InEventPort::getImpl()
{
    return impl;
}

QString InEventPort::toString()
{
    return QString("InEventPort[%1]: "+this->getName()).arg(this->getID());

}

bool InEventPort::isEdgeLegal(GraphMLContainer *attachableObject)
{
    return true;
}

bool InEventPort::isAdoptLegal(GraphMLContainer *child)
{
    return true;
}

