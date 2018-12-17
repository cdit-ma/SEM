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
    broker.AttachData(this, "comment", QVariant::String, ProtectedState::UNPROTECTED);
    broker.AttachData(this, "icon_prefix", QVariant::String, ProtectedState::PROTECTED, "EntityIcons");
    broker.AttachData(this, "icon", QVariant::String, ProtectedState::PROTECTED, "ExternalAssembly");

    auto data_middleware = broker.AttachData(this, "middleware", QVariant::String, ProtectedState::UNPROTECTED);
    auto data_external = broker.AttachData(this, "blackbox", QVariant::Bool, ProtectedState::UNPROTECTED, false);
    data_middleware->addValidValues({"ZMQ", "TAO", "QPID"});

    in_ = broker.ConstructChildNode(*this, NODE_KIND::PORT_REQUEST_DELEGATE);

    //Disable Edge kinds
    broker.SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE, false);
    broker.SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET, false);

    broker.AttachData(in_, "label", QVariant::String, ProtectedState::PROTECTED, "Requests");
    broker.AttachData(in_, "middleware", QVariant::String, ProtectedState::PROTECTED);

    Data::LinkData(this, "middleware", in_, "middleware", true);

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

    return EventPortAssembly::canAcceptEdge(edge_kind, dst);;
}

void MEDEA::ExternalServerDelegate::MiddlewareUpdated(){
    const auto& middleware = getDataValue("middleware").toString();
    const auto& external = getDataValue("blackbox").toBool();

    auto topic_key = getFactoryBroker().GetKey("topic_name", QVariant::String);

    QSet<Node*> elements = {this, in_};

    if(middleware != "QPID"){
        for(auto node : elements){
            getFactoryBroker().RemoveData(node, "topic_name");
        }
    }else{
        for(auto node : elements){
            getFactoryBroker().AttachData(node, topic_key, ProtectedState::UNPROTECTED);
        }
        Data::LinkData(this, "topic_name", in_, "topic_name", true);
    }

    //Add Data
    if(external){
        if(middleware == "TAO"){
            for(auto node : elements){
                getFactoryBroker().AttachData(node, "tao_server_name", QVariant::String, ProtectedState::UNPROTECTED, "context/server_name");
                getFactoryBroker().AttachData(node, "tao_server_kind", QVariant::String, ProtectedState::UNPROTECTED, "kind");
                getFactoryBroker().AttachData(node, "tao_naming_service_endpoint", QVariant::String, ProtectedState::UNPROTECTED, "corbaloc:iiop:IP:PORT");
            }
            Data::LinkData(this, "tao_server_name", in_, "tao_server_name", true);
            Data::LinkData(this, "tao_naming_service_endpoint", in_, "tao_naming_service_endpoint", true);
            Data::LinkData(this, "tao_server_kind", in_, "tao_server_kind", true);
        }else if(middleware == "QPID"){
            for(auto node : elements){
                getFactoryBroker().AttachData(node, "qpid_broker_address", QVariant::String, ProtectedState::UNPROTECTED, "IP:PORT");
            }
            Data::LinkData(this, "qpid_broker_address", in_, "qpid_broker_address", true);
        }else if(middleware == "ZMQ"){
            for(auto node : elements){
                getFactoryBroker().AttachData(node, "zmq_server_address", QVariant::String, ProtectedState::UNPROTECTED, "tcp://IP:PORT");
            }
            Data::LinkData(this, "zmq_server_address", in_, "zmq_server_address", true);
        }
    }else{
        for(auto node : elements){
            getFactoryBroker().RemoveData(node, "tao_server_name");
            getFactoryBroker().RemoveData(node, "tao_naming_service_endpoint");
            getFactoryBroker().RemoveData(node, "tao_server_kind");
            getFactoryBroker().RemoveData(node, "zmq_server_address");
            getFactoryBroker().RemoveData(node, "qpid_broker_address");
        }
    }

    getFactoryBroker().AttachData(this, "icon", QVariant::String, ProtectedState::PROTECTED, (external ? "ManagementComponent" : "ExternalAssembly"));
    getFactoryBroker().AttachData(this, "icon_prefix", QVariant::String, ProtectedState::PROTECTED, "EntityIcons");

    getFactoryBroker().SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::TARGET, true);
    getFactoryBroker().SetAcceptsEdgeKind(in_, EDGE_KIND::ASSEMBLY, EDGE_DIRECTION::SOURCE, !external);
}