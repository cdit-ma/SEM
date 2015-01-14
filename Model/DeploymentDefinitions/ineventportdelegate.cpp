#include "ineventportdelegate.h"
#include "outeventportdelegate.h"
#include "ineventportinstance.h"
#include "outeventportinstance.h"
#include "../edge.h"
#include <QDebug>
#include <typeinfo>

InEventPortDelegate::InEventPortDelegate():Node(Node::NT_NODE)
{
    //qDebug() << "Constructed InEventPortIDL: "<< this->getName();
}

InEventPortDelegate::~InEventPortDelegate()
{
}

bool InEventPortDelegate::connectedToInEventPortInstance()
{
    return getInEventPortInstance() != 0;
}

InEventPortInstance *InEventPortDelegate::getInEventPortInstance()
{
    foreach(Edge* edge, this->getEdges(0)){
        Node* src = edge->getSource();
        Node* dst = edge->getDestination();

        Node* connectedNode = src;
        if(src == this){
            connectedNode = dst;
        }

        InEventPortInstance* iEPI = dynamic_cast<InEventPortInstance*>(connectedNode);
        InEventPortDelegate* iEPD = dynamic_cast<InEventPortDelegate*>(connectedNode);
        if(iEPI){
            return iEPI;
        }
        if(iEPD){
            return iEPD->getInEventPortInstance();
        }
    }
    return 0;

}


bool InEventPortDelegate::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}

//An InEventPortDelegate can be connected to:
//Connected to:
// + InEventPortInstance (If this has no connected Instances already, or it matches the instances deifintino)
// + inEventPortDelegate (If it has no definition already, or it matches the definition)
bool InEventPortDelegate::canConnect(Node* attachableObject)
{
    InEventPortInstance* inEventPortInstance = dynamic_cast<InEventPortInstance*>(attachableObject);
    InEventPortDelegate* inEventPortDelegate = dynamic_cast<InEventPortDelegate*>(attachableObject);


    if(!inEventPortInstance && !inEventPortDelegate){
        qCritical() << "InEventPortDelegate can only be connected to InEventPortInstances (Which match the same definition) and defined InEventPortDelegates.";
        return false;
    }

    if(inEventPortDelegate && !inEventPortDelegate->connectedToInEventPortInstance()){
        qCritical() << "InEventPortDelegate cannot be connected to undefined InEventPortDelegates.";
        return false;
    }
    if(inEventPortDelegate){
        if(!getParentNode()->isAncestorOf(inEventPortDelegate)){
            qCritical() << "InEventPortDelegate cannot be connected to an InEventPortDelegates which isn't contained by the node.";
            return false;
        }

    }

    /*if(inEventPortInstance && connectedToInEventPortInstance()){
        if(this->connectedToInEventPortInstance()){
            qCritical() << "InEventPortDelegate cannot be connected to InEventPortDelegates.";
            return false;
        }
    }
    */

    if(inEventPortDelegate || inEventPortInstance){
        InEventPortInstance* thisInstance = getInEventPortInstance();
        InEventPortInstance* connectingInstance = 0;

        if(inEventPortInstance){
            connectingInstance = inEventPortInstance;
        }
        if(inEventPortDelegate){
            connectingInstance = inEventPortDelegate->getInEventPortInstance();
        }

        if(thisInstance && connectingInstance){
            EventPort* thisDefinition = dynamic_cast<EventPort*>(thisInstance->getDefinition());
            EventPort* connectingDefinition = dynamic_cast<EventPort*>(connectingInstance->getDefinition());

            if(!thisDefinition || !connectingDefinition){
                qWarning() << "InEventPortDelegate cannot be connected to an undefined inEventPortInstance/Delegate.";
                return false;
            }

            if(thisDefinition->getAggregate() != connectingDefinition->getAggregate() && (connectingDefinition->getAggregate() != 0)){
                qWarning() << "InEventPortDelegate cannot be connected to an inEventPortInstance/Delegate with a different Aggregate definition.";
                return false;
            }
        }
    }

    if(inEventPortDelegate){
        if(inEventPortDelegate->getParentNode() == this->getParentNode()){
            qWarning() << "InEventPortDelegate cannot be connected to an InEventPortDelegate owned by the same Assembly!";
            return false;
        }
    }

    if(inEventPortInstance){
        if(!getParentNode()->isAncestorOf(inEventPortInstance)){
            qWarning() << "InEventPortDelegate cannot be connected to an inEventPortInstance not contained in the same Assembly!";
            return false;
        }
    }

    return Node::canConnect(attachableObject);
}
