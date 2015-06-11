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
#ifdef DEBUG_MODE
        qWarning() << "Cannot connect an OutEventPortDelegate until it is defined!";
#endif
        return false;
    }

    if(!inEventPortDelegate && !outEventPortDelegate && !inEventPortInstance){
#ifdef DEBUG_MODE
        qWarning() << "OutEventPortDelegate can only be connected to OutEventPortDelegate, inEventPortInstance and defined InEventPortDelegates.";
#endif
        return false;
    }


    if(inEventPortDelegate){
        if(!inEventPortDelegate->connectedToInEventPortInstance()){
#ifdef DEBUG_MODE
            qWarning() << "OutEventPortDelegate cannot be connected to undefined inEventPortDelegate.";
#endif
            //Do check for topics.
            return false;
        }
        if(inEventPortDelegate->getParentNode()->getParentNode() != this->getParentNode()->getParentNode()){
#ifdef DEBUG_MODE
            qWarning() << "OutEventPortDelegate cannot be connected to an InEventPortDelegate which is not is not on the same level.";
#endif
            return false;
        }
        if(inEventPortDelegate->getParentNode() == this->getParentNode()){
#ifdef DEBUG_MODE
            qWarning() << "OutEventPortDelegate cannot be connected to an InEventPortDelegate owned by the same parent!";
#endif
            return false;
        }

    }
    if(inEventPortInstance){
        if(inEventPortInstance->getParentNode()->getParentNode() != this->getParentNode()->getParentNode()){
#ifdef DEBUG_MODE
            qWarning() << "OutEventPortDelegate cannot be connected to an inEventPortInstance not directly owned by the Delegates Assembly.";
#endif
            return false;
        }
        if(inEventPortInstance->getParentNode()->getParentNode() == this->getParentNode()){
#ifdef DEBUG_MODE
            qWarning() << "OutEventPortDelegate cannot be connected to an inEventPortInstance owned by the same Assembly.!";
#endif
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
#ifdef DEBUG_MODE
            qWarning() << "OutEventPortDelegate cannot be connected to an undefined inEventPortInstance/Delegate.";
#endif

            return false;
        }

        if(oEP->getAggregate() != iEP->getAggregate()  && (iEP->getAggregate() != 0)){
#ifdef DEBUG_MODE
            qWarning() << "OutEventPortDelegate cannot be connected to an inEventPortInstance/Delegate with a different Aggregate definition.";
#endif
            return false;
        }
    }

    if(outEventPortDelegate){
        if(outEventPortDelegate->getParentNode() == this->getParentNode()){

#ifdef DEBUG_MODE
            qWarning() << "OutEventPortDelegate cannot be connected to an OutEventPortDelegate owned by the same Assembly!";
#endif
            return false;
        }
        if(outEventPortDelegate->getParentNode() != this->getParentNode()->getParentNode()){
#ifdef DEBUG_MODE
            qWarning() << "OutEventPortDelegate cannot be connected to an OutEventPortDelegate not contained by the parent Assembly.";
#endif
            return false;
        }

        EventPort* oEP = 0;
        EventPort* oEP2 = 0;

        if(this->getOutEventPortInstance()){
            oEP = dynamic_cast<EventPort*>(this->getOutEventPortInstance()->getDefinition());
        }
        if(outEventPortDelegate->getOutEventPortInstance()){
            oEP2 = dynamic_cast<EventPort*>(outEventPortDelegate->getOutEventPortInstance()->getDefinition());
        }

        if(oEP != oEP2 && (oEP2 != 0)){
#ifdef DEBUG_MODE
            qWarning() << "OutEventPortDelegate cannot be connected to an OutEventPortDelegate Which doesn't share the same definition.";
#endif
            return false;
        }


    }

    return Node::canConnect(attachableObject);
}
