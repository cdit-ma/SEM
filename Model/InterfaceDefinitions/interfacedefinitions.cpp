#include "interfacedefinitions.h"
#include <QDebug>

InterfaceDefinitions::InterfaceDefinitions(QString name): Node()
{

}

InterfaceDefinitions::~InterfaceDefinitions()
{

}

QString InterfaceDefinitions::toString()
{
    return QString("InterfaceDefinitions[%1]: "+this->getName()).arg(this->getID());
}

bool InterfaceDefinitions::canConnect(Node* attachableObject)
{
    return false;
}

bool InterfaceDefinitions::canAdoptChild(Node *child)
{
    return true;

}
