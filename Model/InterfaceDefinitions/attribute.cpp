#include "attribute.h"
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
    return Node::canConnect(attachableObject);
}

bool Attribute::canAdoptChild(Node *child)
{
    return true;

}
