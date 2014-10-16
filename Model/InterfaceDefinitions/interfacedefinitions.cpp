#include "interfacedefinitions.h"
#include <QDebug>

InterfaceDefinitions::InterfaceDefinitions(QString name): Node(name)
{

}

InterfaceDefinitions::~InterfaceDefinitions()
{

}

QString InterfaceDefinitions::toString()
{
    return QString("InterfaceDefinitions[%1]: "+this->getName()).arg(this->getID());
}

bool InterfaceDefinitions::isEdgeLegal(GraphMLContainer *attachableObject)
{
    return false;
}

bool InterfaceDefinitions::isAdoptLegal(GraphMLContainer *child)
{
    return true;

}
