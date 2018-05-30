#include "eventport.h"
#include "aggregate.h"
#include "aggregateinstance.h"
#include "../Keys/typekey.h"
#include "../../entityfactorybroker.h"


EventPort::EventPort(EntityFactoryBroker& broker, NODE_KIND node_kind, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::EVENTPORT);
    setAcceptsEdgeKind(EDGE_KIND::AGGREGATE, EDGE_DIRECTION::SOURCE);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
};

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


Aggregate* EventPort::getAggregate(){
    //Special case.
    if(isInstance()){
        auto definition = getDefinition(true);

        if(definition && definition->isNodeOfType(NODE_TYPE::EVENTPORT)){
            return ((EventPort*)definition)->getAggregate();
        }
    }
    return this->aggregate;
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
    if(getChildrenCount() > 0){
        return false;
    }
    
    return Node::canAdoptChild(child);
}

bool EventPort::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::AGGREGATE:{
        if(getDefinition()){
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
    return Node::canAcceptEdge(edge_kind, dst);
}
