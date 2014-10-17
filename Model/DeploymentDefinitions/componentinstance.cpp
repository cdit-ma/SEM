#include "componentinstance.h"
#include <QDebug>
#include "eventport.h"
#include "outeventportinstance.h"
#include "../BehaviourDefinitions/periodicevent.h"
#include "../BehaviourDefinitions/componentimpl.h"
#include "../InterfaceDefinitions/component.h"
#include "hardwarenode.h"

ComponentInstance::ComponentInstance(QString name):Node(name)
{
    //qDebug() << "Constructed ComponentInstance: "<< this->getName();
    def = 0;
}

ComponentInstance::~ComponentInstance()
{
    if(def){
        def->removeInstance(this);
    }
    //THIS IS A TEST
    //Destructor
}

void ComponentInstance::setDefinition(Component *def)
{
    this->def = def;
}

Component *ComponentInstance::getDefinition()
{
    return def;
}

ComponentImpl *ComponentInstance::getImpl()
{
    if(def){
        return def->getImpl();
    }
    return 0;
}

bool ComponentInstance::isAdoptLegal(GraphMLContainer *attachableObject)
{
    /*

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
    */

    return true;
}

bool ComponentInstance::isEdgeLegal(GraphMLContainer *attachableObject)
{
    HardwareNode* hardwareNode = dynamic_cast<HardwareNode*> (attachableObject);

    ComponentImpl* component = dynamic_cast<ComponentImpl*> (attachableObject);

    if(hardwareNode == 0 && component == 0){
        qWarning() << "ComponentInstance Node can only be connected to a HardwareNode";
        return false;
    }

    //Check for an edge to a component.
    foreach(Edge* edge, this->edges){
        ComponentImpl* src = dynamic_cast<ComponentImpl*>(edge->getSource());
        ComponentImpl* dst = dynamic_cast<ComponentImpl*>(edge->getDestination());
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

