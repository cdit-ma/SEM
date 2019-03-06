#include "eventportassembly.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

EventPortAssembly::EventPortAssembly(EntityFactoryBroker& broker, NODE_KIND node_kind, bool is_temp, bool is_pubsub_port) : EventPort(broker, node_kind, is_temp, is_pubsub_port){
    setNodeType(NODE_TYPE::EVENTPORT_ASSEMBLY);
    
    SetEdgeRuleActive(EdgeRule::IGNORE_REQUIRED_INSTANCE_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE, false);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
};

bool EventPortAssembly::isSubPortDelegate() const
{
    return getNodeKind() == NODE_KIND::PORT_SUBSCRIBER_DELEGATE;
}

bool EventPortAssembly::isPubPortDelegate() const
{
    return getNodeKind() == NODE_KIND::PORT_PUBLISHER_DELEGATE;
}



bool EventPortAssembly::isPubSubPortDelegate() const{
    return getNodeKind() == NODE_KIND::PORT_PUBSUB_DELEGATE;
}

bool EventPortAssembly::isSubPortInstance() const
{
    return getNodeKind() == NODE_KIND::PORT_SUBSCRIBER_INST;
}

bool EventPortAssembly::isPubPortInstance() const
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
        if(!getPortType()){
            return false;
        }

        EventPortAssembly* port = (EventPortAssembly*) dst;

        //Can't connect different aggregates
        if(getPortType() != port->getPortType()){
            return false;
        }

        //Can't connect different aggregates
        if(isPubSubPort() != port->isPubSubPort()){
            return false;
        }

        //Can't connect different aggregates
        if(isReqRepPort() != port->isReqRepPort()){
            return false;
        }

        if(isPubSubPort()){
            int src_depth_to_ancestor = getDepthFromCommonAncestor(port);
            int dst_depth_to_ancestor = port->getDepthFromCommonAncestor(this);
            int depth_delta = src_depth_to_ancestor - dst_depth_to_ancestor;
            int total_depth = src_depth_to_ancestor + dst_depth_to_ancestor;

            if(total_depth > 4){
                //Can't ever go outside our Assembly
                return false;
            }

            bool valid = false;

            if(isPubPortInstance()){
                if(port->isSubPortInstance()){
                    //Only Sub Ports in our same Component Assembly
                    valid = src_depth_to_ancestor == 2 && depth_delta == 0;
                }else if(port->isPubPortDelegate()){
                    //Only Pub Delegates within our Component Assembly
                    valid = src_depth_to_ancestor == 2 && depth_delta == 1;
                }else if(port->isSubPortDelegate()){
                    //Only Sub Delegates At the Same Depth as Us within our Component Assembly
                    valid = src_depth_to_ancestor == 2 && depth_delta == 0;
                }else if(port->isPubSubPortDelegate()){
                    //Only PubSub Delegates in our same Component Assembly
                    valid = src_depth_to_ancestor == 2 && depth_delta == 0;
                }
            }else if(isPubPortDelegate()){
                if(port->isSubPortInstance()){
                    //Only Sub Ports in our same Component Assembly
                    valid = src_depth_to_ancestor == 2 && depth_delta == 0;
                }else if(port->isPubPortDelegate()){
                    //Only Pub Delegates within our Component Assembly
                    valid = src_depth_to_ancestor == 2 && depth_delta == 1;
                }else if(port->isSubPortDelegate()){
                    //Only Sub Delegates within our Component Assembly
                    valid = src_depth_to_ancestor == 2 && depth_delta == 0;
                }else if(port->isPubSubPortDelegate()){
                    //Only PubSub Delegates
                    valid = src_depth_to_ancestor == 2 && depth_delta == 0;
                }
            }else if(isSubPortDelegate()){
                if(port->isSubPortInstance()){
                    //Only Sub Ports in our same Component Assembly
                    valid = src_depth_to_ancestor == 1 && depth_delta == -1;
                }else if(port->isPubPortDelegate()){
                    //Only Pub Delegates within our Component Assembly
                    valid = src_depth_to_ancestor == 1 && depth_delta == 0;
                }else if(port->isSubPortDelegate()){
                    //Only Sub Delegates In other Assemblies
                    valid = src_depth_to_ancestor == 1 && depth_delta == -1;
                }
            }else if(isPubSubPortDelegate()){
                if(port->isSubPortInstance()){
                    //Only Sub Ports in our same Component Assembly
                    valid = src_depth_to_ancestor == 2 && depth_delta == 0;
                }else if(port->isSubPortDelegate()){
                    //Only Sub Delegates within our Component Assembly
                    valid = src_depth_to_ancestor == 2 && depth_delta == 0;
                }
            }
            if(!valid){
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
    QSet<QString> topic_middlewares = {"RTI", "OSPL", "QPID"};


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