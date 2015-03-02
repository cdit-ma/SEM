#include "interfacedefinitions.h"
#include "file.h"
#include <QDebug>
InterfaceDefinitions::InterfaceDefinitions(): Node()
{
}

InterfaceDefinitions::~InterfaceDefinitions()
{

}

bool InterfaceDefinitions::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool InterfaceDefinitions::canAdoptChild(Node *child)
{
    File* file = dynamic_cast<File*>(child);

    if(!file){
#ifdef DEBUG_MODE
        qWarning() << "InterfaceDefinitions can only adopt a File Node";
#endif
        return false;
    }

    return Node::canAdoptChild(child);

}
