#include "outeventportimpl.h"
#include "../InterfaceDefinitions/outeventport.h"

OutEventPortImpl::OutEventPortImpl():EventPortImpl(false){
}

OutEventPortImpl::~OutEventPortImpl(){
}

bool OutEventPortImpl::canConnect_DefinitionEdge(Node *definition)
{
    OutEventPort* outEventPort = dynamic_cast<OutEventPort*>(definition);

    if(!outEventPort){
        return false;
    }

    return EventPortImpl::canConnect_DefinitionEdge(definition);
}

