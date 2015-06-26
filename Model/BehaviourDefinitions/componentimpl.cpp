#include "componentimpl.h"
#include "../InterfaceDefinitions/component.h"
#include "attributeimpl.h"
#include "ineventportimpl.h"
#include "outeventportimpl.h"
#include "../InterfaceDefinitions/memberinstance.h"
#include "../InterfaceDefinitions/idl.h"

#include "branchstate.h"
#include "periodicevent.h"
#include "termination.h"
#include "variable.h"
#include "workload.h"

#include "condition.h"
#include "process.h"

#include <QDebug>

ComponentImpl::ComponentImpl(QString name):Node(Node::NT_IMPL)
{
}

ComponentImpl::~ComponentImpl()
{
}

bool ComponentImpl::canConnect(Node* attachableObject)
{
    Component* component = dynamic_cast<Component*>(attachableObject);

    if(!component){
        return false;
    }

    if(getDefinition()){
#ifdef DEBUG_MODE
        qWarning() << "ComponentImpl already has a definition already";
#endif
        return false;
    }
    if(component->getImplementations().count() != 0){
#ifdef DEBUG_MODE
        qWarning() << "ComponentImpl cannot be connected to a Component which already has an Implementation.";
#endif
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool ComponentImpl::canAdoptChild(Node *child)
{
    BranchState* branchState = dynamic_cast<BranchState*>(child);
    PeriodicEvent* periodicEvent = dynamic_cast<PeriodicEvent*>(child);
    Termination* termination = dynamic_cast<Termination*>(child);
    Variable* variable = dynamic_cast<Variable*>(child);
    Workload* workload = dynamic_cast<Workload*>(child);


    OutEventPortImpl* outEventPortImpl = dynamic_cast<OutEventPortImpl*>(child);
    InEventPortImpl* inEventPortImpl = dynamic_cast<InEventPortImpl*>(child);
    AttributeImpl* attributeImpl = dynamic_cast<AttributeImpl*>(child);

    if(!(branchState || periodicEvent || termination || variable || workload || outEventPortImpl || inEventPortImpl || attributeImpl)){
#ifdef DEBUG_MODE
        qWarning() << "ComponentImpl cannot adopt anything outside of Condition, MemberInstance or Process";
#endif
        return false;
    }

    return Node::canAdoptChild(child);
}


