#include "attribute.h"
#include "../BehaviourDefinitions/attributeimpl.h"
#include "../DeploymentDefinitions/attributeinstance.h"



Attribute::Attribute(QString name):Node(name, Node::NT_DEFINITION)
{
    impl = 0;
}

Attribute::~Attribute()
{

}

void Attribute::addInstance(AttributeInstance *instance)
{
    if(!instances.contains(instance)){
        instances.append(instance);
        instance->setDefinition(this);
    }
}

void Attribute::removeInstance(AttributeInstance *instance)
{
    int index = instances.indexOf(instance);
    if(index >= 0){
        instance->setDefinition(0);
        instances.removeAt(index);
    }
}

QVector<AttributeInstance *> Attribute::getInstances()
{
    return instances;

}

void Attribute::setImpl(AttributeImpl *impl)
{
    this->impl = impl;
    impl->setDefinition(this);

}

AttributeImpl *Attribute::getImpl()
{
    return this->impl;
}

QString Attribute::toString()
{
    return QString("Attribute[%1]: "+this->getName()).arg(this->getID());
}

bool Attribute::canConnect(Node* attachableObject)
{
    return true;
}

bool Attribute::canAdoptChild(Node *child)
{
    return true;

}
