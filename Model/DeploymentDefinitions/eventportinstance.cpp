#include "eventportinstance.h"
#include "../InterfaceDefinitions/aggregateinstance.h"

EventPortInstance::EventPortInstance(bool isInEventPortInstance):Node(Node::NT_INSTANCE)
{
    this->inEventPortInstance = isInEventPortInstance;
    addValidEdgeType(Edge::EC_ASSEMBLY);
    addValidEdgeType(Edge::EC_DEFINITION);
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
    return Node::canConnect_AssemblyEdge(node);
}

bool EventPortInstance::canConnect_DefinitionEdge(Node *definition)
{
    return Node::canConnect_DefinitionEdge(definition);
}
