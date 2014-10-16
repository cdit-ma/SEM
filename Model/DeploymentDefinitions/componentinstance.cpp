#include "componentinstance.h"
#include <QDebug>
#include "eventport.h"
#include "ineventportidl.h"
#include "outeventportidl.h"
#include "../Workload/periodicevent.h"
#include "../Workload/component.h"
#include "hardwarenode.h"
#include "../Workload/component.h"
#include "attribute.h"

ComponentInstance::ComponentInstance(QString name):Node(name)
{
    //qDebug() << "Constructed ComponentInstance: "<< this->getName();
    parentComponent = 0;
}

ComponentInstance::~ComponentInstance()
{
    if(parentComponent != 0){
        parentComponent->removeComponentInstance(this);
    }
    //THIS IS A TEST
    //Destructor
}

void ComponentInstance::setComponentParent(Component *parent)
{
    if(parentComponent == 0){
        parentComponent = parent;
    }
}

Component *ComponentInstance::getComponentParent()
{
    return parentComponent;
}

bool ComponentInstance::isAdoptLegal(GraphMLContainer *attachableObject)
{

    EventPort* eventPort = dynamic_cast<EventPort*> (attachableObject);
    Attribute* attribute = dynamic_cast<Attribute*> (attachableObject);
    PeriodicEvent* period = dynamic_cast<PeriodicEvent*> (attachableObject);

    if(eventPort != 0 || attribute != 0 || period != 0){
        //Do Event Port Specific things!
    }else{
        qWarning() << "ComponentInstance can only adopt an EventPort or an Attribute Node";;
        return false;
    }

    if(this->getGraph() != NULL){
        return this->getGraph()->isAdoptLegal(attachableObject);
    }

    return true;
}

bool ComponentInstance::isEdgeLegal(GraphMLContainer *attachableObject)
{
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*> (attachableObject);

    Component* component = dynamic_cast<Component*> (attachableObject);

    if(hardwareNode == 0 && component == 0){
        qWarning() << "ComponentInstance Node can only be connected to a HardwareNode";
        return false;
    }

    //Check for an edge to a component.
    foreach(Edge* edge, this->edges){
        Component* src = dynamic_cast<Component*>(edge->getSource());
        Component* dst = dynamic_cast<Component*>(edge->getDestination());
        if(src != 0 || dst != 0){
            //Already connection to a Component. Return 0
            qWarning() << "ComponentInstance Node can only be connected to one Component.";
            return false;
        }
    }

    //Check for existing connection.
    if(isConnected(attachableObject)){
        qWarning() << "Already connected to this Object";
        return false;
    }

    return true;
}


QString ComponentInstance::toString()
{
    return QString("ComponentInstance[%1]: "+this->getName()).arg(this->getID());
}

