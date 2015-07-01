#include "attribute.h"
#include <QDebug>
#include "../BehaviourDefinitions/attributeimpl.h"
#include "../DeploymentDefinitions/attributeinstance.h"



Attribute::Attribute():Node(Node::NT_DEFINITION)
{
}

Attribute::~Attribute()
{

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
