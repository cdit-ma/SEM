#include "attributeinstance.h"
#include "../InterfaceDefinitions/attribute.h"
#include "../DeploymentDefinitions/attributeinstance.h"
#include <QDebug>

AttributeInstance::AttributeInstance(QString name): Node(name, Node::NT_INSTANCE)
{
    //qDebug() << "Constructed Attribute: "<< this->getName();
}

AttributeInstance::~AttributeInstance()
{
    //Destructor
}


//An attribute cannot adopt anything.
bool AttributeInstance::isAdoptLegal(GraphMLContainer *child)
{
    Q_UNUSED(child);
    return false;
}


QString AttributeInstance::toString()
{
    return QString("AttributeInstance[%1]: "+this->getName()).arg(this->getID());
}

bool AttributeInstance::isEdgeLegal(GraphMLContainer *attachableObject)
{
    return true;
}

