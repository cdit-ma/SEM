#include "eventportimpl.h"
#include "ineventportimpl.h"
#include "outeventportimpl.h"
#include "../InterfaceDefinitions/aggregateinstance.h"

EventPortImpl::EventPortImpl(bool isInEventPort):BehaviourNode(Node::NT_IMPL)
{
    inEventPort = isInEventPort;
    addValidEdgeType(Edge::EC_WORKFLOW);
    addValidEdgeType(Edge::EC_DEFINITION);
}

EventPortImpl::~EventPortImpl()
{

}

bool EventPortImpl::isInEventPort()
{
    return inEventPort;
}

bool EventPortImpl::isOutEventPort()
{
    return !inEventPort;
}

bool EventPortImpl::canAdoptChild(Node *child)
{
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);

    if(!aggregateInstance){
        //EventPortImpls can only adopt AggregateInstances
        return false;
    }

    if(hasChildren()){
        //EventPortImpls can only adopt 1 AggregateInstance
        return false;
    }

    return Node::canAdoptChild(child);
}

bool EventPortImpl::canConnect_DefinitionEdge(Node *definition)
{
    return Node::canConnect_DefinitionEdge(definition);
}


