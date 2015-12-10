#include "componentimpl.h"

#include "../InterfaceDefinitions/component.h"

#include "attributeimpl.h"
#include "branchstate.h"
#include "ineventportimpl.h"
#include "outeventportimpl.h"
#include "periodicevent.h"
#include "termination.h"
#include "variable.h"
#include "workload.h"
#include "whileloop.h"

ComponentImpl::ComponentImpl():Node(Node::NT_IMPL){
    setAcceptEdgeClass(Edge::EC_DEFINITION);
}

ComponentImpl::~ComponentImpl(){}

bool ComponentImpl::canAdoptChild(Node *child)
{
    AttributeImpl* attributeImpl = dynamic_cast<AttributeImpl*>(child);
    BranchState* branchState = dynamic_cast<BranchState*>(child);
    InEventPortImpl* inEventPortImpl = dynamic_cast<InEventPortImpl*>(child);
    OutEventPortImpl* outEventPortImpl = dynamic_cast<OutEventPortImpl*>(child);
    PeriodicEvent* periodicEvent = dynamic_cast<PeriodicEvent*>(child);
    Termination* termination = dynamic_cast<Termination*>(child);
    Variable* variable = dynamic_cast<Variable*>(child);
    Workload* workload = dynamic_cast<Workload*>(child);
    WhileLoop* whileLoop = dynamic_cast<WhileLoop*>(child);

    if(!(attributeImpl || branchState || inEventPortImpl || outEventPortImpl || periodicEvent || termination || variable || workload || whileLoop)){
        return false;
    }

    return Node::canAdoptChild(child);
}

bool ComponentImpl::canConnect_DefinitionEdge(Node *definition)
{
    Component* component = dynamic_cast<Component*>(definition);

    if(!component){
        return false;
    }

    if(!component->getImplementations().isEmpty()){
        return false;
    }

    return Node::canConnect_DefinitionEdge(definition);
}

