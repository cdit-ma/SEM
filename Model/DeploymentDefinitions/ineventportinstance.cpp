#include "ineventportinstance.h"
#include "../InterfaceDefinitions/ineventport.h"

InEventPortInstance::InEventPortInstance():EventPortInstance(true)
{
}

InEventPortInstance::~InEventPortInstance()
{
}

bool InEventPortInstance::canAdoptChild(Node*)
{
    return false;
}

bool InEventPortInstance::canConnect_DefinitionEdge(Node *definition)
{
    InEventPort* inEventPort = dynamic_cast<InEventPort*>(definition);

    if(!inEventPort){
        return false;
    }

    return EventPortInstance::canConnect_DefinitionEdge(definition);
}

bool InEventPortInstance::canConnect_AssemblyEdge(Node*)
{
    return false;
}
