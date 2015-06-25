#include "componentimpl.h"
#include "../InterfaceDefinitions/component.h"
#include "attributeimpl.h"
#include "ineventportimpl.h"
#include "outeventportimpl.h"
#include "../InterfaceDefinitions/memberinstance.h"
#include "../InterfaceDefinitions/idl.h"

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
    Condition* condition = dynamic_cast<Condition*>(child);
    MemberInstance* memberInstance = dynamic_cast<MemberInstance*>(child);
    Process* process = dynamic_cast<Process*>(child);
    IDL* idl = dynamic_cast<IDL*>(child);

    if(condition || memberInstance || process || idl){
        #ifdef DEBUG_MODE
        qWarning() << "ComponentImpl cannot adopt anything outside of Condition, MemberInstance or Process";
#endif
        return false;
    }


    return Node::canAdoptChild(child);
}


