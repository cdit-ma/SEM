#include "component.h"
#include "periodicevent.h"
#include "../Assembly/componentinstance.h"
#include "../Assembly/attribute.h"
#include "../Assembly/eventport.h"

#include <QDebug>

Component::Component(QString name):Node(name)
{
     qDebug() << "Constructed Component: " << this->getName();
}

Component::~Component()
{
    foreach(ComponentInstance* child, componentInstances){
        delete child;
    }
}

QString Component::toString()
{
    return QString("Component[%1]: "+this->getName()).arg(this->getID());
}

void Component::addComponentInstance(ComponentInstance *componentInstance)
{
    if(componentInstance == 0){
       qCritical() << "Cannot adopt a child Component Instance";
    }else{
        if(!componentInstances.contains(componentInstance)){
            componentInstances.append(componentInstance);
            componentInstance->setComponentParent(this);
        }
    }
}

void Component::removeComponentInstance(ComponentInstance *child)
{
    if(componentInstances.contains(child)){
        componentInstances.removeAt(componentInstances.indexOf(child));
    }
}

QVector<ComponentInstance *> Component::getComponentInstances()
{
    return componentInstances;
}

bool Component::isEdgeLegal(GraphMLContainer *attachableObject)
{
    Q_UNUSED(attachableObject);

    ComponentInstance* componentInstance = dynamic_cast<ComponentInstance*> (attachableObject);

    if(componentInstance == 0){
        qWarning() << "Component Node can only be connected to a Component Instance";
        return false;
    }

    QString newType = attachableObject->getDataValue("type");

    if(newType == ""){
        qWarning() << "Cannot connect a node with no type!";
        return false;
    }

    //Check for an edge to a component.
    foreach(Edge* edge, this->edges){
        ComponentInstance* src = dynamic_cast<ComponentInstance*>(edge->getSource());
        ComponentInstance* dst = dynamic_cast<ComponentInstance*>(edge->getDestination());
        if(src != 0){
            if(src->getDataValue("type") != newType){
                qWarning() << "Cannot connect to a Component Instance with a different Type!";
                return false;
            }
        }
        if(dst != 0){
            if(dst->getDataValue("type") != newType){
                qWarning() << "Cannot connect to a Component Instance with a different Type!";
                return false;
            }
        }
    }


    return true;

}

bool Component::isAdoptLegal(GraphMLContainer *child)
{
    return true;
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
}

void Component::addEdge(Edge *edge)
{
    //Make sure if we are restoring a
    ComponentInstance* src = dynamic_cast<ComponentInstance*>(edge->getSource());
    ComponentInstance* dst = dynamic_cast<ComponentInstance*>(edge->getDestination());
    if(src != 0){
        this->addComponentInstance(src);
    }
    if(dst != 0){
        this->addComponentInstance(dst);
    }

    Node::addEdge(edge);

}

