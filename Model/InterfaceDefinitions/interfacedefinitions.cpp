#include "interfacedefinitions.h"
#include "file.h"
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
    File* file = dynamic_cast<File*>(child);

    if(!file){
        qWarning() << "InterfaceDefinitions can only adopt a File Node";
        return false;
    }

    return Node::canAdoptChild(child);

}
