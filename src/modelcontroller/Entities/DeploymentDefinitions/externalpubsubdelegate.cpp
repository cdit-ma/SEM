#include "externalpubsubdelegate.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"


const NODE_KIND node_kind = NODE_KIND::EXTERNAL_PUBSUB_DELEGATE;
const QString kind_string = "External PubSub Delegate";

void MEDEA::ExternalPubSubDelegate::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new ExternalPubSubDelegate(broker, is_temp_node);
        });
}

MEDEA::ExternalPubSubDelegate::ExternalPubSubDelegate(EntityFactoryBroker& broker, bool is_temp) : EventPortAssembly(broker, node_kind, is_temp, true){
    //Setup State
    setAcceptsNodeKind(NODE_KIND::PORT_PUBSUB_DELEGATE);
    
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE, false);
    setAcceptsEdgeKind(EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET, false);
    setLabelFunctional(false);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "comment", QVariant::String, ProtectedState::UNPROTECTED);
    broker.AttachData(this, "icon_prefix", QVariant::String, ProtectedState::PROTECTED, "EntityIcons");
    broker.AttachData(this, "icon", QVariant::String, ProtectedState::PROTECTED, "ExternalAssembly");

    auto data_middleware = broker.AttachData(this, "middleware", QVariant::String, ProtectedState::UNPROTECTED);
    auto data_external = broker.AttachData(this, "blackbox", QVariant::Bool, ProtectedState::UNPROTECTED, false);
    data_middleware->addValidValues({"ZMQ", "RTI", "OSPL", "QPID"});

    in_ = broker.ConstructChildNode(*this, NODE_KIND::PORT_PUBSUB_DELEGATE);

    //Disable Edge kinds
    broker.SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE, false);
    broker.SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET, false);

    broker.AttachData(in_, "label", QVariant::String, ProtectedState::PROTECTED, "Events");
    broker.AttachData(in_, "middleware", QVariant::String, ProtectedState::PROTECTED);

    LinkData(this, "middleware", in_, "middleware", true);

    connect(data_middleware, &Data::dataChanged, this, &MEDEA::ExternalPubSubDelegate::MiddlewareUpdated);
    connect(data_external, &Data::dataChanged, this, &MEDEA::ExternalPubSubDelegate::MiddlewareUpdated);
    
    MiddlewareUpdated();
}


bool MEDEA::ExternalPubSubDelegate::canAdoptChild(Node *node)
{
    auto node_kind = node->getNodeKind();
    if(getChildrenOfKindCount(node_kind) > 0){
        return false;
    }
    return Node::canAdoptChild(node);
}

bool MEDEA::ExternalPubSubDelegate::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    
    switch(edge_kind){
    case EDGE_KIND::AGGREGATE:{
        bool in_shared_datatype = false;

        for(auto parent : dst->getParentNodes()){
            if(parent->getNodeKind() == NODE_KIND::SHARED_DATATYPES){
                in_shared_datatype = true;
                break;
            }
        }

        if(!in_shared_datatype){
            return false;
        }
        break;
    }
    default:
        break;
    }

    return EventPortAssembly::canAcceptEdge(edge_kind, dst);;
}

void MEDEA::ExternalPubSubDelegate::MiddlewareUpdated(){
    const auto& middleware = getDataValue("middleware").toString();
    const auto& external = getDataValue("blackbox").toBool();

    QSet<QString> topic_middlewares = {"RTI", "OSPL", "QPID"};

    auto topic_key = getFactoryBroker().GetKey("topic_name", QVariant::String);

    QSet<Node*> elements = {this, in_};

    if(!topic_middlewares.contains(middleware)){
        for(auto node : elements){
            getFactoryBroker().RemoveData(node, "topic_name");
        }
    }else{
        for(auto node : elements){
            getFactoryBroker().AttachData(node, topic_key, ProtectedState::UNPROTECTED);
        }
        LinkData(this, "topic_name", in_, "topic_name", true);
    }
    bool allow_inputs = true;

    //Add Data
    if(external){
        if(middleware == "RTI" || middleware == "OSPL"){
            for(auto node : elements){
                getFactoryBroker().AttachData(node, "dds_domain_id", QVariant::Int, ProtectedState::UNPROTECTED, 0);
                getFactoryBroker().RemoveData(node, "zmq_publisher_address");
                getFactoryBroker().RemoveData(node, "qpid_broker");
            }
            LinkData(this, "dds_domain_id", in_, "dds_domain_id", true);
            
        }else if(middleware == "ZMQ"){
            for(auto node : elements){
                getFactoryBroker().AttachData(node, "zmq_publisher_address", QVariant::String, ProtectedState::UNPROTECTED, "tcp://IP:PORT");
                getFactoryBroker().RemoveData(node, "dds_domain_id");
                getFactoryBroker().RemoveData(node, "qpid_broker");
            }
            allow_inputs = false;
            LinkData(this, "zmq_publisher_address", in_, "zmq_publisher_address", true);
        }else if(middleware == "QPID"){
            for(auto node : elements){
                getFactoryBroker().AttachData(node, "qpid_broker", QVariant::String, ProtectedState::UNPROTECTED, "IP:PORT");
                getFactoryBroker().RemoveData(node, "zmq_publisher_address");
                getFactoryBroker().RemoveData(node, "dds_domain_id");
            }
            LinkData(this, "qpid_broker", in_, "qpid_broker", true);
        }
    }else{
        for(auto node : elements){
            getFactoryBroker().RemoveData(node, "zmq_publisher_address");
            getFactoryBroker().RemoveData(node, "dds_domain_id");
            getFactoryBroker().RemoveData(node, "qpid_broker");
        }
    }

    getFactoryBroker().AttachData(this, "icon", QVariant::String, ProtectedState::PROTECTED, (external ? "ManagementComponent" : "ExternalAssembly"));
    getFactoryBroker().AttachData(this, "icon_prefix", QVariant::String, ProtectedState::PROTECTED, "EntityIcons");

    getFactoryBroker().SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE, true);
    getFactoryBroker().SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET, allow_inputs);
}