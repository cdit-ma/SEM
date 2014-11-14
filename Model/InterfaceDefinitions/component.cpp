#include "component.h"
#include "../BehaviourDefinitions/componentimpl.h"
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

    if(componentImpl && getImplementation()){
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

    if(!outEventPort && !inEventPort && !attribute){
        qWarning() << "Can only adopt an OutEventPort, an InEventPort or an Attribute in a Component.";
        return false;
    }

    return Node::canAdoptChild(child);
}
