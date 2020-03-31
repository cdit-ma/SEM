#include "externalserverdelegate.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"


const NODE_KIND node_kind = NODE_KIND::EXTERNAL_SERVER_DELEGATE;
const QString kind_string = "External Server Delegate";

void MEDEA::ExternalServerDelegate::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new ExternalServerDelegate(broker, is_temp_node);
        });
}

MEDEA::ExternalServerDelegate::ExternalServerDelegate(EntityFactoryBroker& broker, bool is_temp) : EventPortAssembly(broker, node_kind, is_temp, false){
    //Setup State
    setAcceptsNodeKind(NODE_KIND::PORT_REQUEST_DELEGATE);
    
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
    data_middleware->addValidValues({"ZMQ", "TAO", "QPID"});

    in_ = broker.ConstructChildNode(*this, NODE_KIND::PORT_REQUEST_DELEGATE);

    //Disable Edge kinds
    broker.SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE, false);
    broker.SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET, false);

    broker.AttachData(in_, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "Requests");
    broker.AttachData(in_, KeyName::Middleware, QVariant::String, ProtectedState::PROTECTED);
    Data::LinkData(this, KeyName::Middleware, in_, KeyName::Middleware, true);

    QSet<Node*> elements = {this, in_};

    for(auto node : elements){
        broker.AttachData(node, KeyName::TaoServerName, QVariant::String, ProtectedState::PROTECTED, "context/server_name");
        broker.AttachData(node, KeyName::TaoServerKind, QVariant::String, ProtectedState::PROTECTED, "kind");
        broker.AttachData(node, KeyName::TaoNamingServiceEndpoint, QVariant::String, ProtectedState::PROTECTED, "corbaloc:iiop:IP:PORT");
        broker.AttachData(node, KeyName::QpidBrokerAddress, QVariant::String, ProtectedState::PROTECTED, "IP:PORT");
        broker.AttachData(node, KeyName::ZmqServerAddress, QVariant::String, ProtectedState::PROTECTED, "tcp://IP:PORT");
        broker.AttachData(node, KeyName::TopicName, QVariant::String, ProtectedState::PROTECTED, "");
    }

    //Bind the Data
    Data::LinkData(this, KeyName::TaoServerName, in_, KeyName::TaoServerName, true);
    Data::LinkData(this, KeyName::TaoNamingServiceEndpoint, in_, KeyName::TaoNamingServiceEndpoint, true);
    Data::LinkData(this, KeyName::TaoServerKind, in_, KeyName::TaoServerKind, true);
    Data::LinkData(this, KeyName::QpidBrokerAddress, in_, KeyName::QpidBrokerAddress, true);
    Data::LinkData(this, KeyName::ZmqServerAddress, in_, KeyName::ZmqServerAddress, true);
    Data::LinkData(this, KeyName::TopicName, in_, KeyName::TopicName, true);

    connect(data_middleware, &Data::dataChanged, this, &MEDEA::ExternalServerDelegate::MiddlewareUpdated);
    connect(data_external, &Data::dataChanged, this, &MEDEA::ExternalServerDelegate::MiddlewareUpdated);
    
    MiddlewareUpdated();
}


bool MEDEA::ExternalServerDelegate::canAdoptChild(Node *node)
{
    auto node_kind = node->getNodeKind();
    if(getChildrenOfKindCount(node_kind) > 0){
        return false;
    }
    return Node::canAdoptChild(node);
}

bool MEDEA::ExternalServerDelegate::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    
    switch(edge_kind){
    case EDGE_KIND::AGGREGATE:{
        bool in_shared_datatype = false;

        if(dst->getNodeKind() == NODE_KIND::SERVER_INTERFACE){
            for(auto parent : dst->getParentNodes(-1)){
                if(parent->getNodeKind() == NODE_KIND::SHARED_DATATYPES){
                    in_shared_datatype = true;
                    break;
                }
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

    return EventPortAssembly::canAcceptEdge(edge_kind, dst);
}

void MEDEA::ExternalServerDelegate::MiddlewareUpdated()
{
    const auto& external = getDataValue(KeyName::BlackBox).toBool();
    const auto& middleware = getDataValue(KeyName::Middleware).toString();

    getFactoryBroker().AttachData(this, KeyName::Icon, QVariant::String, ProtectedState::PROTECTED, (external ? "ManagementComponent" : "ExternalAssembly"));
    getFactoryBroker().AttachData(this, KeyName::IconPrefix, QVariant::String, ProtectedState::PROTECTED, "EntityIcons");

    const auto zmq_options_state =  (external && middleware == "ZMQ") ? ProtectedState::UNPROTECTED : ProtectedState::PROTECTED;
    const auto qpid_options_state = (external && middleware == "QPID") ? ProtectedState::UNPROTECTED : ProtectedState::PROTECTED;
    const auto tao_options_state =  (external && middleware == "TAO") ? ProtectedState::UNPROTECTED : ProtectedState::PROTECTED;

    getFactoryBroker().AttachData(this, KeyName::TaoServerName, QVariant::String, tao_options_state);
    getFactoryBroker().AttachData(this, KeyName::TaoServerKind, QVariant::String, tao_options_state);
    getFactoryBroker().AttachData(this, KeyName::TaoNamingServiceEndpoint, QVariant::String, tao_options_state);
    getFactoryBroker().AttachData(this, KeyName::QpidBrokerAddress, QVariant::String, qpid_options_state);
    getFactoryBroker().AttachData(this, KeyName::TopicName, QVariant::String, middleware == "QPID" ? ProtectedState::UNPROTECTED : ProtectedState::PROTECTED);
    getFactoryBroker().AttachData(this, KeyName::ZmqServerAddress, QVariant::String, zmq_options_state);

    getFactoryBroker().SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET, true);
    getFactoryBroker().SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE, !external);
}