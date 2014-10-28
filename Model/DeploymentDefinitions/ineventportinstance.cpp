#include "ineventportinstance.h"
#include <QDebug>
#include <typeinfo>
#include "outeventportinstance.h"
#include "../InterfaceDefinitions/ineventport.h"

InEventPortInstance::InEventPortInstance(QString name):Node(Node::NT_INSTANCE)
{
    //qDebug() << "Constructed InEventPortIDL: "<< this->getName();
}

InEventPortInstance::~InEventPortInstance()
{
   // foreach(InEventPort* child, inEventPorts){
    //    delete child;
   // }

}


bool InEventPortInstance::canAdoptChild(Node *child)
{
    return false;
}

bool InEventPortInstance::canConnect(Node* attachableObject)
{
    OutEventPortInstance* outputEventPort = dynamic_cast<OutEventPortInstance*> (attachableObject);

    InEventPort* inEventPort = dynamic_cast<InEventPort*>(attachableObject);

    if(outputEventPort == 0 && inEventPort == 0){
        qCritical() << "Cannot connect an IEPI to anything which isn't an EventPort instance";
        return false;
    }

    if(inEventPort && getDefinition()){
        qCritical() << "Cannot connect an IEPI to more than 1 Definition";
        return false;
    }

    if(outputEventPort){
        if(!outputEventPort->getDefinition()){
            qCritical() << "Cannot connect an IEPI to a un-defined EventPort!";
            return false;
        }

        EventPort* eventPortSrc = dynamic_cast<EventPort*>(outputEventPort->getDefinition());
        EventPort* eventPortDst = dynamic_cast<EventPort*>(getDefinition());

        if(eventPortSrc && eventPortDst){
            if(!eventPortSrc->getAggregate()){
                qWarning() << "Cannot connect an IEPI to an un-Aggregated EventPort";
                return false;
            }
            if(eventPortSrc->getAggregate() != eventPortDst->getAggregate()){
                qWarning() << "Cannot connect an IEPI 2 EventPort Instances which have a differing Aggregate";
                return false;
            }

        }
    }

    return Node::canConnect(attachableObject);
}


QString InEventPortInstance::toString()
{
    return QString("InEventPortInstance[%1]: "+this->getName()).arg(this->getID());

}


