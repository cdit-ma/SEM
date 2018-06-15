#include "eventport.h"
#include "aggregate.h"
#include "aggregateinstance.h"
#include "../Keys/typekey.h"
#include "../../entityfactorybroker.h"


EventPort::EventPort(EntityFactoryBroker& broker, NODE_KIND node_kind, bool is_temp, bool is_pubsub_port) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::EVENTPORT);
    setAcceptsEdgeKind(EDGE_KIND::AGGREGATE, EDGE_DIRECTION::SOURCE);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);

    this->is_pubsub_port = is_pubsub_port;

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
};

bool EventPort::isInPort() const
{
    return getNodeKind() == NODE_KIND::PORT_SUBSCRIBER;
}

bool EventPort::isOutPort() const
{
    return getNodeKind() == NODE_KIND::PORT_PUBLISHER;
}

bool EventPort::isPubSubPort() const{
    return is_pubsub_port;
}

bool EventPort::isReqRepPort() const{
    return !is_pubsub_port;
}


void EventPort::setPortType(Node* port_type){
    if(!getPortType()){
        this->port_type = port_type;
        //Do binding!
        TypeKey::BindTypes(port_type, this, true);
    }
}


Node* EventPort::getPortType(){
    //Special case.
    if(isInstance()){
        auto definition = getDefinition(true);

        if(definition && definition->isNodeOfType(NODE_TYPE::EVENTPORT)){
            return ((EventPort*)definition)->getPortType();
        }
    }
    return this->port_type;
}

void EventPort::unsetPortType()
{
    if(port_type){
        TypeKey::BindTypes(port_type, this, false);
        port_type = 0;
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

        if(getPortType()){
            //Don't allow multiple instances.
            return false;
        }

        auto dst_kind = dst->getNodeKind();

        if(isPubSubPort() && dst_kind != NODE_KIND::AGGREGATE){
            return false;
        }

        if(isReqRepPort() && dst_kind != NODE_KIND::SERVER_INTERFACE){
            return false;
        }

        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
}
