#include "eventportinstance.h"
#include "../InterfaceDefinitions/aggregateinstance.h"
#include "eventportdelegate.h"

EventPortInstance::EventPortInstance(bool isInEventPortInstance):Node(Node::NT_INSTANCE)
{
    this->inEventPortInstance = isInEventPortInstance;
    setAcceptEdgeClass(Edge::EC_ASSEMBLY);
    setAcceptEdgeClass(Edge::EC_DEFINITION);
    setAcceptEdgeClass(Edge::EC_QOS);
}

EventPortInstance::~EventPortInstance()
{

}

bool EventPortInstance::isInEventPortInstance()
{
    return inEventPortInstance;
}

bool EventPortInstance::isOutEventPortInstance()
{
    return !inEventPortInstance;
}


Aggregate *EventPortInstance::getAggregate()
{
    if(getEventPortDefinition()){
        return getEventPortDefinition()->getAggregate();
    }
    return 0;
}

EventPort *EventPortInstance::getEventPortDefinition()
{
    EventPort* eventPort = 0;
    if(getDefinition()){
        eventPort = (EventPort*) getDefinition();
    }
    return eventPort;
}

bool EventPortInstance::canAdoptChild(Node *child)
{
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);

    if(!aggregateInstance){
        //EventPortInstances can only adopt AggregateInstances
        return false;
    }

    if(hasChildren()){
        //EventPortInstances can only adopt 1 AggregateInstance
        return false;
    }

    return Node::canAdoptChild(child);
}

bool EventPortInstance::canConnect_AssemblyEdge(Node *node)
{
    EventPortInstance* eventPortInstance = dynamic_cast<EventPortInstance*>(node);
    EventPortDelegate* eventPortDelegate = dynamic_cast<EventPortDelegate*>(node);

    if(!(eventPortInstance || eventPortDelegate)){
        return false;
    }

    EventPort* portDefinition = getEventPortDefinition();
    if(!portDefinition){
        //Can only connect assembly edges once this has a definition.
        return false;
    }

    Aggregate* portAggregate = getEventPortDefinition()->getAggregate();
    if(!portAggregate){
        //Can only connect assembly edges once this has a definition which is connected to an aggregate.
        return false;
    }

    //Get the ComponentAssembly which contains this.
    Node* thisAssembly = getParentNode(2);

    Node* nodeAssembly = 0;

    if(eventPortInstance){
        EventPort* eventPortDefinition = eventPortInstance->getEventPortDefinition();
        if(!eventPortDefinition){
            //Can only connect to EventPortInstance's which have Definitions.
            return false;
        }

        if(!eventPortDefinition->getAggregate()){
            //Can only connect to EventPortInstance's which Definitions have aggregates.
            return false;
        }

        if(eventPortDefinition->getAggregate() != portAggregate){
            //EventPorts must have the same aggregates.
            return false;
        }

        if(eventPortInstance->isInEventPortInstance() == isInEventPortInstance()){
            //Can't have matching port connections.
            return false;
        }

        nodeAssembly = node->getParentNode(2);
    }

    if(eventPortDelegate){
        nodeAssembly = node->getParentNode();
        if(!eventPortDelegate->getAggregate()){
            return false;
        }
        if(portAggregate != eventPortDelegate->getAggregate()){
            return false;
        }
        if(isInEventPortInstance() != eventPortDelegate->isInEventPortDelegate()){
            return false;
        }
    }

    if(thisAssembly != nodeAssembly){
        //Assembly connections must share the same parent
        return false;
    }


    return Node::canConnect_AssemblyEdge(node);
}

bool EventPortInstance::canConnect_DefinitionEdge(Node *definition)
{
    return Node::canConnect_DefinitionEdge(definition);
}
