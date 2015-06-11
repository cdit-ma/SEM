#include "interfacedefinitions.h"
#include "idl.h"
#include "blackbox.h"

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
    IDL* file = dynamic_cast<IDL*>(child);
    BlackBox* blackBox = dynamic_cast<BlackBox*>(child);
    if(!file && !blackBox){
#ifdef DEBUG_MODE
        qWarning() << "InterfaceDefinitions can only adopt a File or BlackBox Node";
#endif
        return false;
    }

    return Node::canAdoptChild(child);

}
