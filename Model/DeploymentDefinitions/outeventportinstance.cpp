#include "outeventportinstance.h"
#include "../InterfaceDefinitions/outeventport.h"
#include "eventportdelegate.h"

OutEventPortInstance::OutEventPortInstance():EventPortInstance(false)
{
}

OutEventPortInstance::~OutEventPortInstance()
{
}

bool OutEventPortInstance::canAdoptChild(Node*)
{
    return false;
}

bool OutEventPortInstance::canConnect_DefinitionEdge(Node *definition)
{
    OutEventPort* outEventPort = dynamic_cast<OutEventPort*>(definition);

    if(!outEventPort){
        return false;
    }

    return EventPortInstance::canConnect_DefinitionEdge(definition);
}

bool OutEventPortInstance::canConnect_AssemblyEdge(Node *node)
{
    return EventPortInstance::canConnect_AssemblyEdge(node);
}
