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
    broker.AttachData(this, KeyName::Comment, QVariant::String, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::IconPrefix, QVariant::String, ProtectedState::PROTECTED, "EntityIcons");
    broker.AttachData(this, KeyName::Icon, QVariant::String, ProtectedState::PROTECTED, "ExternalAssembly");

    auto data_middleware = broker.AttachData(this, KeyName::Middleware, QVariant::String, ProtectedState::UNPROTECTED);
    auto data_external = broker.AttachData(this, KeyName::BlackBox, QVariant::Bool, ProtectedState::UNPROTECTED, false);
    data_middleware->addValidValues({"ZMQ", "RTI", "OSPL", "QPID"});

    in_ = broker.ConstructChildNode(*this, NODE_KIND::PORT_PUBSUB_DELEGATE);

    //Disable Edge kinds
    broker.SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE, false);
    broker.SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET, false);

    broker.AttachData(in_, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "Events");
    broker.AttachData(in_, KeyName::Middleware, QVariant::String, ProtectedState::PROTECTED);

    Data::LinkData(this, KeyName::Middleware, in_, KeyName::Middleware, true);

    QSet<Node*> elements = {this, in_};
    for(auto node : elements){
        broker.AttachData(node, KeyName::DdsDomainID, QVariant::Int, ProtectedState::PROTECTED, 0);
        broker.AttachData(node, KeyName::ZmqPublisherAddress, QVariant::String, ProtectedState::PROTECTED, "IP:PORT");
        broker.AttachData(node, KeyName::QpidBrokerAddress, QVariant::String, ProtectedState::PROTECTED, "tcp://IP:PORT");
        broker.AttachData(node, KeyName::TopicName, QVariant::String, ProtectedState::PROTECTED);
    }

    Data::LinkData(this, KeyName::DdsDomainID, in_, KeyName::DdsDomainID, true);
    Data::LinkData(this, KeyName::ZmqPublisherAddress, in_, KeyName::ZmqPublisherAddress, true);
    Data::LinkData(this, KeyName::QpidBrokerAddress, in_, KeyName::QpidBrokerAddress, true);
    Data::LinkData(this, KeyName::TopicName, in_, KeyName::TopicName, true);

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

void MEDEA::ExternalPubSubDelegate::MiddlewareUpdated()
{
    const auto& middleware = getDataValue(KeyName::Middleware).toString();
    const auto& external = getDataValue(KeyName::BlackBox).toBool();

    static const QSet<QString> topic_middlewares{"RTI", "OSPL", "QPID"};

    const auto dds_options_state =   (external && (middleware == "RTI" || middleware == "OSPL"))? ProtectedState::UNPROTECTED : ProtectedState::PROTECTED;
    const auto zmq_options_state =   (external && middleware == "ZMQ") ? ProtectedState::UNPROTECTED : ProtectedState::PROTECTED;
    const auto qpid_options_state =  (external && middleware == "QPID") ? ProtectedState::UNPROTECTED : ProtectedState::PROTECTED;
    const auto topic_options_state = topic_middlewares.contains(middleware) ? ProtectedState::UNPROTECTED : ProtectedState::PROTECTED;

    getFactoryBroker().AttachData(this, KeyName::DdsDomainID, QVariant::Int, dds_options_state);
    getFactoryBroker().AttachData(this, KeyName::ZmqPublisherAddress, QVariant::String, zmq_options_state);
    getFactoryBroker().AttachData(this, KeyName::QpidBrokerAddress, QVariant::String, qpid_options_state);
    getFactoryBroker().AttachData(this, KeyName::TopicName, QVariant::String, topic_options_state);
    getFactoryBroker().AttachData(this, KeyName::Icon, QVariant::String, ProtectedState::PROTECTED, (external ? "ManagementComponent" : "ExternalAssembly"));
    getFactoryBroker().AttachData(this, KeyName::IconPrefix, QVariant::String, ProtectedState::PROTECTED, "EntityIcons");

    bool allow_inputs = true;
    if (external && middleware == "ZMQ") {
        allow_inputs = false;
    }

    getFactoryBroker().SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE, true);
    getFactoryBroker().SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET, allow_inputs);
}