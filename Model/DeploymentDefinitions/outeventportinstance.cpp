#include "outeventportinstance.h"

#include "../node.h"
#include "ineventportinstance.h"
#include <QDebug>

OutEventPortInstance::OutEventPortInstance(QString name):Node(Node::NT_INSTANCE)
{
    //qDebug() << "Constructed OutEventPortIDL: "<< this->getName();
}

OutEventPortInstance::~OutEventPortInstance()
{

}


bool OutEventPortInstance::canAdoptChild(Node *child)
{
    return false;
}

bool OutEventPortInstance::canConnect(Node* attachableObject)
{
    InEventPortInstance* inputEventPort = dynamic_cast<InEventPortInstance*> (attachableObject);

    OutEventPort* outEventPort = dynamic_cast<OutEventPort*>(attachableObject);

    if(!inputEventPort && !outEventPort){
        qCritical() << "Cannot connect an OEPI to anything which isn't an EventPort instance";
        return false;
    }

    if(outEventPort && getDefinition()){
        qCritical() << "Cannot connect an OEPI to more than 1 Definition";
        return false;
    }

    if(inputEventPort){
        if(!inputEventPort->getDefinition()){
            qCritical() << "Cannot connect an OEPI to a un-defined EventPort!";
            return false;
        }

        EventPort* eventPortSrc = dynamic_cast<EventPort*>(inputEventPort->getDefinition());
        EventPort* eventPortDst = dynamic_cast<EventPort*>(getDefinition());

        if(eventPortSrc && eventPortDst){
            if(!eventPortSrc->getAggregate()){
                //qWarning() << "Cannot connect an OEPI an un-Aggregated EventPort";
                //return false;
            }
            if(eventPortSrc->getAggregate() != eventPortDst->getAggregate()){
                qWarning() << "Cannot connect an OEPI 2 EventPort Instances which have a differing Aggregate";
                //return false;
            }
        }
    }

    return Node::canConnect(attachableObject);

}

QString OutEventPortInstance::toString()
{
    return QString("OutEventPortInstance[%1]: "+this->getName()).arg(this->getID());
}
