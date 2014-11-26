#include "componentimpl.h"
#include "../InterfaceDefinitions/component.h"

#include <QDebug>

ComponentImpl::ComponentImpl(QString name):Node(Node::NT_IMPL)
{
}

ComponentImpl::~ComponentImpl()
{
}

QString ComponentImpl::toString()
{
    return QString("ComponentImpl[%1]: "+this->getName()).arg(this->getID());
}


bool ComponentImpl::canConnect(Node* attachableObject)
{
    Component* component = dynamic_cast<Component*>(attachableObject);

    if(getDefinition()){
        qWarning() << "ComponentImpl already has a definition already";
        return false;
    }

    if(!component){
        qWarning() << "ComponentImpl can only connect to a Component.";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool ComponentImpl::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return true;
}


