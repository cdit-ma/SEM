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
    return true;
    /*
    PeriodicEvent* periodicEvent = dynamic_cast<PeriodicEvent*> (child);
    Attribute* attribute = dynamic_cast<Attribute*> (child);
    EventPort* eventPort = dynamic_cast<EventPort*> (child);

    if(periodicEvent != 0 || attribute != 0 || eventPort != 0){
        //Check stuff!
    }else{
        qWarning() << "Component can only connect to either a PeriodicEvent, and attribute or an event port.";
        return false;

    }

    //Check for existing connection.
    if(isConnected(child)){
        qWarning() << "Already connected to this Object";
        return false;
    }

    return true;
    */
}


