#include "eventportassembly.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

EventPortAssembly::EventPortAssembly(EntityFactoryBroker& broker, NODE_KIND node_kind, bool is_temp) : EventPort(broker, node_kind, is_temp){
    setNodeType(NODE_TYPE::EVENTPORT_ASSEMBLY);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET);
    SetEdgeRuleActive(EdgeRule::IGNORE_REQUIRED_INSTANCE_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE, false);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
};

bool EventPortAssembly::isInPortDelegate() const
{
    return getNodeKind() == NODE_KIND::PORT_SUBSCRIBER_DELEGATE;
}
bool EventPortAssembly::isReplierPortInst() const{
    return getNodeKind() == NODE_KIND::PORT_REPLIER_INST;
}
bool EventPortAssembly::isRequesterPortInst() const{
    return getNodeKind() == NODE_KIND::PORT_REQUESTER_INST;
}

bool EventPortAssembly::isOutPortDelegate() const
{
    return getNodeKind() == NODE_KIND::PORT_PUBLISHER_DELEGATE;
}

bool EventPortAssembly::isInPortAssembly() const
{
    return isInPortDelegate() || isInPortInstance();
}

bool EventPortAssembly::isOutPortAssembly() const
{
    return isOutPortDelegate() || isOutPortInstance();

}

bool EventPortAssembly::isPortDelegate() const
{
    return isInPortDelegate() || isOutPortDelegate();
}

bool EventPortAssembly::isPortInstance() const
{
    return isInPortInstance() || isOutPortInstance();
}

bool EventPortAssembly::isInPortInstance() const
{
    return getNodeKind() == NODE_KIND::PORT_SUBSCRIBER_INST;
}

bool EventPortAssembly::isOutPortInstance() const
{
    return getNodeKind() == NODE_KIND::PORT_PUBLISHER_INST;
}

#include <QDebug>

bool EventPortAssembly::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::ASSEMBLY:{
        //Can't connect to something that isn't an EventPortAssembly
        if(!dst->isNodeOfType(NODE_TYPE::EVENTPORT_ASSEMBLY)){
            return false;
        }

        //Can't have an assembly link without an aggregate.
        if(!getAggregate()){
            return false;
        }

        EventPortAssembly* port = (EventPortAssembly*) dst;

        //Can't connect different aggregates
        if(getAggregate() != port->getAggregate()){
            return false;
        }

        int depthToAncestor = getDepthFromCommonAncestor(port);
        int depthToAncestorReverse = port->getDepthFromCommonAncestor(this);
        int delta = depthToAncestor - depthToAncestorReverse;
        int abs_difference = abs(delta);
        int totalDepth = depthToAncestor + depthToAncestorReverse;

        //Can connect in either the same Assembly or 1 different higher.
        if(abs_difference > 1){
            return false;
        }
        if(totalDepth > 4){
            return false;
        }

        if(delta < 0 && !isInPortAssembly()){
            return false;
        }

        if(abs_difference == 0){
            //Different Parents
            if(depthToAncestor == 2){
                //Don't allow connections from the same type, inter assembly.
                if(isOutPortDelegate() && !port->isInPortAssembly()){
                    return false;
                }
                if(isOutPortInstance() && !port->isInPortAssembly()){
                    return false;
                }
            }else if(depthToAncestor == 1){
                if(isPortInstance() && port->isPortInstance()){
                    //Dont allow cycles into the same component
                    return false;
                }else if(isPortDelegate() && port->isPortDelegate()){
                    //No delegate to delegate.
                    return false;
                }else{
                    if(isInPortAssembly() != port->isInPortAssembly()){
                        return false;
                    }
                }
            }
        }else if(abs_difference == 1){
            if(isInPortAssembly() != port->isInPortAssembly()){
                return false;
            }
        }

        if(isOutPortInstance()){
            if(!(port->isInPortInstance() || port->isPortDelegate())){
                return false;
            }
        }
        if(isOutPortDelegate()){
            if(!(port->isInPortInstance() || port->isPortDelegate())){
                return false;
            }
        }
        if(isInPortAssembly()){
            if(port->isInPortAssembly()){
                if(depthToAncestor >= depthToAncestorReverse){
                    return false;
                }
            }else{
                return false;
            }
        }

        break;
    }
    default:
        break;
    }
    return EventPort::canAcceptEdge(edge_kind, dst);
}

void EventPortAssembly::MiddlewareUpdated(){
    const auto& middleware = getDataValue("middleware").toString();

    QSet<QString> qos_middlewares = {"RTI", "OSPL"};
    QSet<QString> topic_middlewares = {"RTI", "OSPL", "AMQP"};


    auto topic_key = getFactoryBroker().GetKey("topic_name", QVariant::String);
    if(topic_middlewares.contains(middleware)){
        if(!gotData(topic_key)){
            getFactoryBroker().AttachData(this, topic_key, ProtectedState::UNPROTECTED);
        }
    }else{
        getFactoryBroker().RemoveData(this, "topic_name");
    }

    setAcceptsEdgeKind(EDGE_KIND::QOS, EDGE_DIRECTION::SOURCE, qos_middlewares.contains(middleware));
}