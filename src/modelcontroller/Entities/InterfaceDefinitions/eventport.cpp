#include "eventport.h"
#include "aggregate.h"
#include "aggregateinstance.h"
#include "../Keys/typekey.h"



EventPort::EventPort(EntityFactory* factory, NODE_KIND kind, QString kind_str) : Node(factory, kind, kind_str){
    RegisterDefaultData(factory, kind, "type", QVariant::String, true);
};

EventPort::EventPort(NODE_KIND kind):Node(kind)
{
    aggregate = 0;
    setNodeType(NODE_TYPE::EVENTPORT);
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::AGGREGATE);

    
    //updateDefaultData("type", QVariant::String, true);
}

bool EventPort::isInPort() const
{
    return getNodeKind() == NODE_KIND::INEVENTPORT;
}

bool EventPort::isOutPort() const
{
    return getNodeKind() == NODE_KIND::OUTEVENTPORT;
}

void EventPort::setAggregate(Aggregate *aggregate)
{
    if(!getAggregate()){
        this->aggregate = aggregate;
        //Do binding!
        TypeKey::BindTypes(aggregate, this, true);
    }
}

Aggregate *EventPort::getAggregate()
{
    //Special case.
    if(isInstance()){
        Node* definition = getDefinition(true);

        if(definition && definition->isNodeOfType(NODE_TYPE::EVENTPORT)){
            return ((EventPort*)definition)->getAggregate();
        }
    }
    return aggregate;
}

void EventPort::unsetAggregate()
{
    if(aggregate){
        TypeKey::BindTypes(aggregate, this, false);
        aggregate = 0;
    }
}

bool EventPort::canAdoptChild(Node *child)
{
    //Can Only accept 1 child.
    if(hasChildren()){
        return false;
    }

    //Can only adopt AggregateInstances
    if(child->getNodeKind() != NODE_KIND::AGGREGATE_INSTANCE){
        return false;
    }

    return Node::canAdoptChild(child);
}

bool EventPort::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case EDGE_KIND::AGGREGATE:{
        if(isNodeOfType(NODE_TYPE::INSTANCE)){
            //Don't allow Instances to have aggregate.
            return false;
        }

        if(getAggregate()){
            //Don't allow multiple instances.
            return false;
        }

        if(dst->getNodeKind() != NODE_KIND::AGGREGATE){
            return false;
        }

        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);
}
