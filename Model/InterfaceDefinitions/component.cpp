#include "component.h"
#include "../BehaviourDefinitions/componentimpl.h"
#include "../DeploymentDefinitions/componentinstance.h"

Component::Component(QString name): Node(name)
{
    impl = 0;
}

Component::~Component()
{

}

void Component::addInstance(ComponentInstance *instance)
{
    if(!instances.contains(instance)){
        instances.append(instance);
        instance->setDefinition(this);
    }
}

void Component::removeInstance(ComponentInstance *instance)
{
    int index = instances.indexOf(instance);
    if(index >= 0){
        instance->setDefinition(0);
        instances.removeAt(index);
    }
}

QVector<ComponentInstance *> Component::getInstances()
{
    return instances;
}

void Component::setImpl(ComponentImpl *impl)
{
    this->impl = impl;
    impl->setDefinition(this);
}

ComponentImpl *Component::getImpl()
{
    return impl;

}

QString Component::toString()
{
     return QString("Component[%1]: "+this->getName()).arg(this->getID());

}

bool Component::isEdgeLegal(GraphMLContainer *attachableObject)
{
    return true;
}

bool Component::isAdoptLegal(GraphMLContainer *child)
{
    return true;
}
