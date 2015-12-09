#include "eventportdelegate.h"
#include "eventportinstance.h"

EventPortDelegate::EventPortDelegate(bool inEventPortDelegate):Node()
{
    this->inEventPortDelegate = inEventPortDelegate;
    addValidEdgeType(Edge::EC_ASSEMBLY);
}

EventPortDelegate::~EventPortDelegate()
{
}

bool EventPortDelegate::isInEventPortDelegate()
{
    return inEventPortDelegate;
}

bool EventPortDelegate::isOutEventPortDelegate()
{
    return !inEventPortDelegate;
}

EventPortInstance *EventPortDelegate::getConnectedEventPortInstance()
{
    foreach(Edge* edge, getEdges(0)){
        Node* src = edge->getSource();
        Node* dst = edge->getDestination();

        Node* otherNode = src;

        if(src == this){
            otherNode = dst;
        }

        EventPortDelegate* eventPortDelegate = dynamic_cast<EventPortDelegate*>(otherNode);
        EventPortInstance* eventPortInstance = dynamic_cast<EventPortInstance*>(otherNode);
        if(eventPortInstance){
            return eventPortInstance;
        }
        if(eventPortDelegate){
            return eventPortDelegate->getConnectedEventPortInstance();
        }
    }
    return 0;
}

bool EventPortDelegate::canAdoptChild(Node*)
{
    return false;
}

bool EventPortDelegate::canConnect_AssemblyEdge(Node *node)
{

}

