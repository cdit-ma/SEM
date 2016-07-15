#include "interfacedefinitions.h"
#include "idl.h"

InterfaceDefinitions::InterfaceDefinitions(): Node(Node::NT_ASPECT)
{
}

InterfaceDefinitions::~InterfaceDefinitions()
{
}

VIEW_ASPECT InterfaceDefinitions::getViewAspect()
{
    return VA_INTERFACES;
}

bool InterfaceDefinitions::canAdoptChild(Node *child)
{
    IDL* idl = dynamic_cast<IDL*>(child);

    if(!idl){
        return false;
    }

    return Node::canAdoptChild(child);
}
