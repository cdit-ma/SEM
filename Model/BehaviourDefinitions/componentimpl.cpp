#include "componentimpl.h"
#include "../InterfaceDefinitions/component.h"

#include <QDebug>

ComponentImpl::ComponentImpl(QString name):Node(Node::NT_IMPL)
{
     //qDebug() << "Constructed Component: " << this->getName();
}

ComponentImpl::~ComponentImpl()
{
    //foreach(ComponentInstance* child, componentInstances){
    //    delete child;
    //}
}

QString ComponentImpl::toString()
{
    return QString("ComponentImpl[%1]: "+this->getName()).arg(this->getID());
}



bool ComponentImpl::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
/*
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
*/

    return false;

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


