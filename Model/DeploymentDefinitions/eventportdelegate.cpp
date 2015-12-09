
#include "eventportdelegate.h"
#include "eventportinstance.h"

EventPortDelegate::EventPortDelegate(bool inEventPortDelegate):Node()
{
    this->inEventPortDelegate = inEventPortDelegate;
    this->aggregate = 0;
    addValidEdgeType(Edge::EC_AGGREGATE);
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

Aggregate *EventPortDelegate::getAggregate()
{
    return aggregate;
}

bool EventPortDelegate::canAdoptChild(Node*)
{
    return false;
}

bool EventPortDelegate::canConnect_AssemblyEdge(Node *node)
{
    EventPortDelegate* eventPortDelegate = dynamic_cast<EventPortDelegate*>(node);
    EventPortInstance* eventPortInstance = dynamic_cast<EventPortInstance*>(node);

    if(!getAggregate()){
        return false;
    }

    if(!(eventPortDelegate || eventPortInstance)){
        return false;
    }

    //Get the ComponentAssembly which contains this.
    Node* thisParent = getParentNode();
    Node* thisParentParent = getParentNode(1);
    Node* nodeParent = node->getParentNode();
    Node* nodeParentParent = node->getParentNode(1);

    Aggregate* portAggregate = getAggregate();

    if(eventPortDelegate){
        if(thisParentParent != nodeParentParent){
            //Check for 1 layer deep assembly link.
            if(!(thisParent == nodeParentParent || nodeParent == thisParentParent)){
                return false;
            }
        }

        if(thisParent == nodeParent){
            //Can't chain EventPortDelegates in the same parent assembly together if they are of the same direction.
            return false;
        }

        if(!eventPortDelegate->getAggregate()){
            return false;
        }
        if(portAggregate != eventPortDelegate->getAggregate()){
            //Can't connect to an eventPortDelegate which has a different Aggregate
            return false;
        }
    }

    if(eventPortInstance){
        if(thisParent != nodeParentParent){
            //Can only connect to EventPortInstances owned by the same assembly as this parent.
            return false;
        }
        if(portAggregate){
            if(portAggregate != eventPortInstance->getAggregate()){
                //Can only connect to EventPortInstances which share the same aggregate.
                return false;
            }
        }
        if(isInEventPortDelegate() != eventPortInstance->isInEventPortInstance()){
            //Can only connect to the same type of EventPortInstance as i am delegate.
            return false;
        }
    }


    return Node::canConnect_AssemblyEdge(node);
}

bool EventPortDelegate::canConnect_AggregateEdge(Node *node)
{
    Aggregate* aggregate = dynamic_cast<Aggregate*>(node);

    if(!aggregate){
        return false;
    }

    if(getAggregate()){
        return false;
    }

    return Node::canConnect_AggregateEdge(aggregate);
}

void EventPortDelegate::setAggregate(Aggregate *aggregate)
{
    if(getAggregate() != aggregate){
        this->aggregate = aggregate;
    }
}

void EventPortDelegate::unsetAggregate()
{
    aggregate = 0;
}
