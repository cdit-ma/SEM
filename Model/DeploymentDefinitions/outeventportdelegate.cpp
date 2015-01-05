#include "outeventportdelegate.h"
#include "ineventportdelegate.h"
#include "ineventportinstance.h"
#include "outeventportinstance.h"
#include "../edge.h"
#include <QDebug>
#include <typeinfo>

OutEventPortDelegate::OutEventPortDelegate():Node(Node::NT_NODE)
{
    //qDebug() << "Constructed InEventPortIDL: "<< this->getName();
}

OutEventPortDelegate::~OutEventPortDelegate()
{
}

bool OutEventPortDelegate::connectedToOutEventPortInstance()
{
    return getOutEventPortInstance() != 0;
}

OutEventPortInstance *OutEventPortDelegate::getOutEventPortInstance()
{
    foreach(Edge* edge, this->getEdges(0)){
        Node* src = edge->getSource();
        Node* dst = edge->getDestination();

        Node* connectedNode = src;
        if(src == this){
            connectedNode = dst;
        }

        OutEventPortInstance* oEPI = dynamic_cast<OutEventPortInstance*>(connectedNode);
        OutEventPortDelegate* oEPD = dynamic_cast<OutEventPortDelegate*>(connectedNode);
        if(oEPI){
            return oEPI;
        }
        if(oEPD){
            return oEPD->getOutEventPortInstance();
        }
    }
    return 0;
}



bool OutEventPortDelegate::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}

bool OutEventPortDelegate::canConnect(Node* attachableObject)
{

    OutEventPortDelegate* outEventPortDelegate = dynamic_cast<OutEventPortDelegate*>(attachableObject);
    InEventPortDelegate* inEventPortDelegate = dynamic_cast<InEventPortDelegate*>(attachableObject);
    InEventPortInstance* inEventPortInstance = dynamic_cast<InEventPortInstance*>(attachableObject);


    if(!connectedToOutEventPortInstance()){
        qCritical() << "Cannot connect an OutEventPortDelegate until it is defined!";
        return false;
    }

    if(!inEventPortDelegate && !outEventPortDelegate && !inEventPortInstance){
        qCritical() << "OutEventPortDelegate can only be connected to OutEventPortDelegate, inEventPortInstance and defined InEventPortDelegates.";
        return false;
    }


    if(inEventPortDelegate){
        if(!inEventPortDelegate->connectedToInEventPortInstance()){
            qCritical() << "OutEventPortDelegate cannot be connected to undefined inEventPortDelegate.";
            //Do check for topics.
            return false;
        }
        if(inEventPortDelegate->getParentNode()->getParentNode() != this->getParentNode()->getParentNode()){
            qCritical() << "OutEventPortDelegate cannot be connected to an InEventPortDelegate which is not is not on the same level.";
            return false;
        }
        if(inEventPortDelegate->getParentNode() == this->getParentNode()){
            qCritical() << "OutEventPortDelegate cannot be connected to an InEventPortDelegate owned by the same parent!";
            return false;
        }

    }
    if(inEventPortInstance){
        if(inEventPortInstance->getParentNode()->getParentNode() != this->getParentNode()->getParentNode()){
            qCritical() << "OutEventPortDelegate cannot be connected to an inEventPortInstance not directly owned by the Delegates Assembly.";
            return false;
        }
        if(inEventPortInstance->getParentNode()->getParentNode() == this->getParentNode()){
            qCritical() << "OutEventPortDelegate cannot be connected to an inEventPortInstance owned by the same Assembly.!";
            return false;
        }
    }

    if(inEventPortDelegate || inEventPortInstance){
        OutEventPortInstance* oEPI = getOutEventPortInstance();
        InEventPortInstance* iEPI = inEventPortInstance;
        if(inEventPortDelegate){
            iEPI = inEventPortDelegate->getInEventPortInstance();
        }

        if(!oEPI || !iEPI){
            qWarning() << "Error 6";
            return false;
        }

        EventPort* oEP = dynamic_cast<EventPort*>(oEPI->getDefinition());
        EventPort* iEP = dynamic_cast<EventPort*>(iEPI->getDefinition());

        if(!oEP || !iEP){
            qWarning() << "OutEventPortDelegate cannot be connected to an undefined inEventPortInstance/Delegate.";
            return false;
        }

        if(oEP->getAggregate() != iEP->getAggregate()  && (iEP->getAggregate() != 0)){
            qWarning() << "OutEventPortDelegate cannot be connected to an inEventPortInstance/Delegate with a different Aggregate definition.";
            return false;
        }
    }

    if(outEventPortDelegate){
        if(outEventPortDelegate->getParentNode() == this->getParentNode()){
            qWarning() << "OutEventPortDelegate cannot be connected to an OutEventPortDelegate owned by the same Assembly!";
            return false;
        }
        if(outEventPortDelegate->getParentNode() != this->getParentNode()->getParentNode()){
            qWarning() << "OutEventPortDelegate cannot be connected to an OutEventPortDelegate not contained by the parent Assembly.";
            return false;
        }
        EventPort* oEP = dynamic_cast<EventPort*>(this->getOutEventPortInstance()->getDefinition());
        EventPort* oEP2 = dynamic_cast<EventPort*>(outEventPortDelegate->getOutEventPortInstance()->getDefinition());

        if(oEP != oEP2){
            qWarning() << "OutEventPortDelegate cannot be connected to an OutEventPortDelegate Which doesn't share the same definition.";
            return false;
        }


    }


    return Node::canConnect(attachableObject);
}


QString OutEventPortDelegate::toString()
{
    return QString("InEventPortDelegate[%1]: " + this->getName()).arg(this->getID());

}
