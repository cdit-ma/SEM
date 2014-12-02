#include "attribute.h"
#include <QDebug>
#include "../BehaviourDefinitions/attributeimpl.h"
#include "../DeploymentDefinitions/attributeinstance.h"



Attribute::Attribute(QString name):Node(Node::NT_DEFINITION)
{
}

Attribute::~Attribute()
{

}


QString Attribute::toString()
{
    return QString("Attribute[%1]: "+this->getName()).arg(this->getID());
}

bool Attribute::canConnect(Node* attachableObject)
{
    AttributeImpl* attributeImpl = dynamic_cast<AttributeImpl*>(attachableObject);
    AttributeInstance* attributeInstance = dynamic_cast<AttributeInstance*>(attachableObject);

    if(!attributeImpl && !attributeInstance){
        qWarning() << "Attribute can only connect to AttributeImpl's and AttributeInstances";
        return false;
    }
    if(attributeImpl && getImplementations().size() != 0){
        qCritical() << "Attribute can only connect to one AttributeImpl";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool Attribute::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}
