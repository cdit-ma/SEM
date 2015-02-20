#include "componentimpl.h"
#include "../InterfaceDefinitions/component.h"
#include "attributeimpl.h"
#include "ineventportimpl.h"
#include "outeventportimpl.h"

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
        qWarning() << "ComponentImpl already has a definition already";
        return false;
    }
    if(component->getImplementations().count() != 0){
        qWarning() << "ComponentImpl cannot be connected to a Component which already has an Implementation.";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool ComponentImpl::canAdoptChild(Node *child)
{
    AttributeImpl* attributeImpl = dynamic_cast<AttributeImpl*>(child);
    InEventPortImpl* inEventPortImpl = dynamic_cast<InEventPortImpl*>(child);
    OutEventPortImpl* outEventPortImpl = dynamic_cast<OutEventPortImpl*>(child);

    if(!attributeImpl && !inEventPortImpl && !outEventPortImpl){
        //qWarning() << "ComponentImpl can Only adopt AttributeImpl, InEventPortImpl and OutEventPortImpl.";
        //return false;

    }

    return Node::canAdoptChild(child);
}


