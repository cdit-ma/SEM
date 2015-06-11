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
#ifdef DEBUG_MODE
        qWarning() << "InEventPortDelegate can only be connected to InEventPortInstances (Which match the same definition) and defined InEventPortDelegates.";
#endif
        return false;
    }

    if(inEventPortDelegate && !inEventPortDelegate->connectedToInEventPortInstance()){
#ifdef DEBUG_MODE
        qWarning() << "InEventPortDelegate cannot be connected to undefined InEventPortDelegates.";
#endif
        return false;
    }
    if(inEventPortDelegate){
        if(!getParentNode()->isAncestorOf(inEventPortDelegate)){
#ifdef DEBUG_MODE
            qWarning() << "InEventPortDelegate cannot be connected to an InEventPortDelegates which isn't contained by the node.";
#endif
            return false;
        }

    }

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
#ifdef DEBUG_MODE
                qWarning() << "InEventPortDelegate cannot be connected to an undefined inEventPortInstance/Delegate.";
#endif
                return false;
            }

            if(thisDefinition->getAggregate() != connectingDefinition->getAggregate() && (connectingDefinition->getAggregate() != 0)){
#ifdef DEBUG_MODE
                qWarning() << "InEventPortDelegate cannot be connected to an inEventPortInstance/Delegate with a different Aggregate definition.";
#endif
                return false;
            }
        }
    }

    if(inEventPortDelegate){
        if(inEventPortDelegate->getParentNode() == this->getParentNode()){
#ifdef DEBUG_MODE
            qWarning() << "InEventPortDelegate cannot be connected to an InEventPortDelegate owned by the same Assembly!";
#endif
            return false;
        }
    }

    if(inEventPortInstance){
        if(inEventPortInstance->getParentNode()){
            if(inEventPortInstance->getParentNode()->getParentNode() != getParentNode()){
                #ifdef DEBUG_MODE
                qWarning() << "InEventPortDelegate cannot be connected to an inEventPortInstance which is not at the same depth!";
                #endif
                return false;
            }
        }
        if(!getParentNode()->isAncestorOf(inEventPortInstance)){
    #ifdef DEBUG_MODE
                qWarning() << "InEventPortDelegate cannot be connected to an inEventPortInstance not contained in the same Assembly!";
    #endif
            return false;
        }
    }

    return Node::canConnect(attachableObject);
}
