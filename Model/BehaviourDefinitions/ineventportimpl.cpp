#include "ineventportimpl.h"
#include "../InterfaceDefinitions/ineventport.h"
#include <QDebug>

InEventPortImpl::InEventPortImpl():EventPortImpl(true){
    setIsWorkflowStart(true);
}

InEventPortImpl::~InEventPortImpl(){
}

bool InEventPortImpl::canConnect_DefinitionEdge(Node *definition)
{
    InEventPort* inEventPort = dynamic_cast<InEventPort*>(definition);

    if(!inEventPort){
        return false;
    }

    return EventPortImpl::canConnect_DefinitionEdge(definition);
}

