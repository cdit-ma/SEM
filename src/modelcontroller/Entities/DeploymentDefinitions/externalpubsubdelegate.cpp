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

    Data::LinkData(this, "middleware", in_, "middleware", true);

    QSet<Node*> elements = {this, in_};
    for(auto node : elements){
        broker.AttachData(node, "dds_domain_id", QVariant::Int, ProtectedState::PROTECTED, 0);
        broker.AttachData(node, "zmq_publisher_address", QVariant::String, ProtectedState::PROTECTED, "IP:PORT");
        broker.AttachData(node, "qpid_broker_address", QVariant::String, ProtectedState::PROTECTED, "tcp://IP:PORT");
        broker.AttachData(node, "topic_name", QVariant::String, ProtectedState::PROTECTED);
    }

    Data::LinkData(this, "dds_domain_id", in_, "dds_domain_id", true);
    Data::LinkData(this, "zmq_publisher_address", in_, "zmq_publisher_address", true);
    Data::LinkData(this, "qpid_broker_address", in_, "qpid_broker_address", true);
    Data::LinkData(this, "topic_name", in_, "topic_name", true);


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

        for(auto parent : dst->getParentNodes(-1)){
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

    static const QSet<QString> topic_middlewares{"RTI", "OSPL", "QPID"};

    const auto dds_options_state =   (external && (middleware == "RTI" || middleware == "OSPL"))? ProtectedState::UNPROTECTED : ProtectedState::PROTECTED;
    const auto zmq_options_state =   (external && middleware == "ZMQ") ? ProtectedState::UNPROTECTED : ProtectedState::PROTECTED;
    const auto qpid_options_state =  (external && middleware == "QPID") ? ProtectedState::UNPROTECTED : ProtectedState::PROTECTED;
    const auto topic_options_state = topic_middlewares.contains(middleware) ? ProtectedState::UNPROTECTED : ProtectedState::PROTECTED;

    getFactoryBroker().AttachData(this, "dds_domain_id", QVariant::Int, dds_options_state);
    getFactoryBroker().AttachData(this, "zmq_publisher_address", QVariant::String, zmq_options_state);
    getFactoryBroker().AttachData(this, "qpid_broker_address", QVariant::String, qpid_options_state);
    getFactoryBroker().AttachData(this, "topic_name", QVariant::String, topic_options_state);


    getFactoryBroker().AttachData(this, "icon", QVariant::String, ProtectedState::PROTECTED, (external ? "ManagementComponent" : "ExternalAssembly"));
    getFactoryBroker().AttachData(this, "icon_prefix", QVariant::String, ProtectedState::PROTECTED, "EntityIcons");


    bool allow_inputs = true;
    if(external && middleware == "ZMQ"){
        allow_inputs = false;
    }

    getFactoryBroker().SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE, true);
    getFactoryBroker().SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET, allow_inputs);
}