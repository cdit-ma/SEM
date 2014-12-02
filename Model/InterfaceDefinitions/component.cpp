#include "component.h"
#include "../BehaviourDefinitions/componentimpl.h"
#include "../BehaviourDefinitions/outeventportimpl.h"
#include "../DeploymentDefinitions/componentinstance.h"
#include "outeventport.h"
#include "ineventport.h"
#include "attribute.h"
#include <QDebug>

Component::Component(QString name): Node(Node::NT_DEFINITION)
{
}

Component::~Component()
{

}


QString Component::toString()
{
     return QString("Component[%1]: "+this->getName()).arg(this->getID());

}

bool Component::canConnect(Node* attachableObject)
{
    ComponentImpl* componentImpl = dynamic_cast<ComponentImpl*>(attachableObject);
    ComponentInstance* componentInstance = dynamic_cast<ComponentInstance*>(attachableObject);

    if(!componentImpl && !componentInstance){
        qWarning() << "Can only connect a Component to a ComponentImpl / ComponentInstance";
        return false;
    }

    if(componentImpl && getImplementations().size() != 0){
        qWarning() << "A Component can only connect to one ComponentImpl. Detach First!";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool Component::canAdoptChild(Node *child)
{
    OutEventPort* outEventPort  = dynamic_cast<OutEventPort*>(child);
    InEventPort* inEventPort  = dynamic_cast<InEventPort*>(child);
    Attribute* attribute  = dynamic_cast<Attribute*>(child);
    OutEventPortImpl* outEventPortImpl = dynamic_cast<OutEventPortImpl*>(child);

    if(!outEventPort && !inEventPort && !attribute && !outEventPortImpl){
        qWarning() << "Can only adopt an OutEventPort, OutEventPortImpl, an InEventPort or an Attribute in a Component.";
        return false;
    }

    return Node::canAdoptChild(child);
}
