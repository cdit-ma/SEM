#include "outeventportinstance.h"
#include "../InterfaceDefinitions/outeventport.h"

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
