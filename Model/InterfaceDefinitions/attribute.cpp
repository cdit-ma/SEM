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
    Q_UNUSED(attachableObject);
    return false;
}

bool Attribute::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}
