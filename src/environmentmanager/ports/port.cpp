#include "port.h"
#include "../node.h"
#include "../component.h"
#include "../environment.h"
#include "../experiment.h"
#include "../attribute.h"

#include "ddsport.h"
#include "taoport.h"
#include "qpidport.h"
#include "zmqport.h"
#include "periodicport.h"

using namespace EnvironmentManager;

std::unique_ptr<Port> Port::ConstructPort(Component& parent, const NodeManager::Port& port){
    try{
        switch(port.middleware()){
            case NodeManager::Middleware::ZMQ:{
                return std::unique_ptr<Port>(new zmq::Port(parent, port));
            }
            case NodeManager::Middleware::QPID:{
                return std::unique_ptr<Port>(new qpid::Port(parent, port));
            }
            case NodeManager::Middleware::TAO:{
                return std::unique_ptr<Port>(new tao::Port(parent, port));
            }
            case NodeManager::Middleware::RTI:
            case NodeManager::Middleware::OSPL:{
                return std::unique_ptr<Port>(new dds::Port(parent, port));
            }
            case NodeManager::Middleware::NO_MIDDLEWARE:{
                if(port.kind() == NodeManager::Port::PERIODIC){
                    return std::unique_ptr<Port>(new PeriodicPort(parent, port));
                }
                break;
            }
            default:
                break;
        }
    }
    catch(const std::exception& ex){
        std::cerr << " * Failed to construct port: " << port.info().name() << parent.GetName() << std::endl;
        std::cerr << ex.what() << std::endl;
    }
    return nullptr;
}

std::unique_ptr<Port> Port::ConstructBlackboxPort(Experiment& parent, const NodeManager::ExternalPort& port){
    try{
        switch(port.middleware()){
            case NodeManager::Middleware::ZMQ:{
                return std::unique_ptr<Port>(new zmq::Port(parent, port));
            }
            case NodeManager::Middleware::QPID:{
                return std::unique_ptr<Port>(new qpid::Port(parent, port));
            }
            case NodeManager::Middleware::TAO:{
                return std::unique_ptr<Port>(new tao::Port(parent, port));
            }
            case NodeManager::Middleware::RTI:
            case NodeManager::Middleware::OSPL:{
                return std::unique_ptr<Port>(new dds::Port(parent, port));
            }
            default:
                break;
        }
    }
    catch(const std::exception& ex){
        std::cerr << " * Failed to construct blackbox port: " << port.info().name() << parent.GetName() << std::endl;
        std::cerr << ex.what() << std::endl;
    }
    return nullptr;
}

Port::Port(Component& parent, const NodeManager::Port& port) :
    experiment_(parent.GetNode().GetExperiment()),
    parent_(&parent)
{
    id_ = port.info().id();
    name_ = port.info().name();
    kind_ = TranslateProtoKind(port.kind());
    middleware_ = TranslateProtoMiddleware(port.middleware());
    blackbox_type_ = Port::BlackboxType::NotBlackbox;

    
    //Append to the list of namespaces
    for(const auto& ns : port.info().namespaces()){
        namespaces_.emplace_back(ns);
    }

    //Use the exact type
    SetType(port.info().type());

    for(const auto& connected_id : port.connected_external_ports()){
        AddExternalConnectedPortId(connected_id);
        switch(kind_){
            case EnvironmentManager::Port::Kind::Publisher:
            case EnvironmentManager::Port::Kind::Replier:{
                GetExperiment().AddExternalProducerPort(connected_id, id_);
                break;
            }
            case EnvironmentManager::Port::Kind::Subscriber:
            case EnvironmentManager::Port::Kind::Requester:{
                GetExperiment().AddExternalConsumerPort(connected_id, id_);
                break;
            }
            default:
                break;
        }
    }

    for(const auto& connected_id : port.connected_ports()){
        AddInternalConnectedPortId(connected_id);
    }
}

Port::Port(Experiment& parent, const NodeManager::ExternalPort& port):
experiment_(parent)
{
    id_ = port.info().id();
    name_ = port.info().name();
    middleware_ = TranslateProtoMiddleware(port.middleware());

    blackbox_type_ = Port::BlackboxType::NotBlackbox;

    if(port.kind() == NodeManager::ExternalPort::SERVER && port.is_blackbox()){
        blackbox_type_ = Port::BlackboxType::ReqRep;
    }
    if(port.kind() == NodeManager::ExternalPort::PUBSUB && port.is_blackbox()){
        blackbox_type_ = Port::BlackboxType::PubSub;
    }
    
    //Append to the list of namespaces
    for(const auto& ns : port.info().namespaces()){
        namespaces_.emplace_back(ns);
    }

    //Use the exact type
    SetType(port.info().type());

    for(const auto& connected_id : port.connected_ports()){
        AddInternalConnectedPortId(connected_id);
    }
}

Port::~Port(){
    //Set if we are external
    for(const auto& connected_id : connected_external_port_ids_){
        switch(kind_){
            case EnvironmentManager::Port::Kind::Publisher:
            case EnvironmentManager::Port::Kind::Replier:{
                GetExperiment().RemoveExternalProducerPort(connected_id, id_);
                break;
            }
            case EnvironmentManager::Port::Kind::Subscriber:
            case EnvironmentManager::Port::Kind::Requester:{
                GetExperiment().RemoveExternalConsumerPort(connected_id, id_);
                break;
            }
            default:
                break;
        }
    }
}

void Port::AddAttribute(const NodeManager::Attribute& attribute){
    attributes_.insert(std::make_pair(attribute.info().id(), 
            std::unique_ptr<EnvironmentManager::Attribute>(
                new EnvironmentManager::Attribute(attribute))));
}

std::string Port::GetId() const{
    return id_;
};
std::string Port::GetName() const{
    return name_;
};
Port::Kind Port::GetKind() const{
    return kind_;
}
Port::Middleware Port::GetMiddleware() const{
    return middleware_;
}

Port::BlackboxType Port::GetBlackboxType() const{
    return blackbox_type_;
}

Component& Port::GetComponent() const{
    if(GotComponent()){
        return *(parent_);
    }
    throw std::runtime_error("No Component");
}

bool Port::GotComponent() const{
    return parent_;
}

Node& Port::GetNode() const{
    if(GotComponent()){
        return parent_->GetNode();
    }
}

Experiment& Port::GetExperiment() const{
    return experiment_;
}

Environment& Port::GetEnvironment() const{
    return GetExperiment().GetEnvironment();
}

void Port::SetType(const std::string& type){
    type_ = type;
}


void Port::AddInternalConnectedPortId(const std::string& port_id){
    connected_internal_port_ids_.insert(port_id);
}

void Port::AddExternalConnectedPortId(const std::string& port_id){
    connected_external_port_ids_.insert(port_id);
}

const std::set<std::string>& Port::GetInternalConnectedPortIds() const{
    return connected_internal_port_ids_;
}

const std::set<std::string>& Port::GetExternalConnectedPortIds() const{
    return connected_external_port_ids_;
}

void Port::SetDirty(){
    dirty_ = true;
    GetComponent().SetDirty();
}

bool Port::IsDirty() const{
    return dirty_;
}

std::unique_ptr<NodeManager::Port> Port::GetProto(const bool full_update){
    std::unique_ptr<NodeManager::Port> port;

    if(dirty_ || full_update){
        port = std::unique_ptr<NodeManager::Port>(new NodeManager::Port());
        
        auto port_info = port->mutable_info();
        port_info->set_name(name_);
        port_info->set_id(id_);
        port_info->set_type(type_);

        port->set_kind(TranslateInternalKind(kind_));
        port->set_middleware(TranslateInternalMiddleware(middleware_));

        for(const auto& ns : namespaces_){
            port_info->add_namespaces(ns);
        }

        for(const auto& connected_port : connected_internal_port_ids_){
            port->add_connected_ports(connected_port);
        }

        for(const auto& external_port : connected_external_port_ids_){
            port->add_connected_external_ports(external_port);
        }

        FillPortPb(*port);
    }
    return port;
}

Port::Kind Port::TranslateProtoKind(const NodeManager::Port::Kind kind){
    const static std::map<NodeManager::Port::Kind, Port::Kind> kind_map({
        {NodeManager::Port::NO_KIND, Port::Kind::NoKind},
        {NodeManager::Port::PERIODIC, Port::Kind::Periodic},
        {NodeManager::Port::PUBLISHER, Port::Kind::Publisher},
        {NodeManager::Port::SUBSCRIBER, Port::Kind::Subscriber},
        {NodeManager::Port::REQUESTER, Port::Kind::Requester},
        {NodeManager::Port::REPLIER, Port::Kind::Replier}});

    if(kind_map.count(kind)){
        return kind_map.at(kind);
    }
    return Port::Kind::NoKind;
}

NodeManager::Port::Kind Port::TranslateInternalKind(const Port::Kind kind){
    const static std::map<Port::Kind, NodeManager::Port::Kind> kind_map({
        {Port::Kind::NoKind, NodeManager::Port::NO_KIND},
        {Port::Kind::Periodic, NodeManager::Port::PERIODIC},
        {Port::Kind::Publisher, NodeManager::Port::PUBLISHER},
        {Port::Kind::Subscriber, NodeManager::Port::SUBSCRIBER},
        {Port::Kind::Requester, NodeManager::Port::REQUESTER},
        {Port::Kind::Replier, NodeManager::Port::REPLIER}});

    if(kind_map.count(kind)){
        return kind_map.at(kind);
    }
    return NodeManager::Port::NO_KIND;
}

Port::Middleware Port::TranslateProtoMiddleware(const NodeManager::Middleware middleware){
    const static std::map<NodeManager::Middleware, Port::Middleware> mw_map({
        {NodeManager::Middleware::NO_MIDDLEWARE, Port::Middleware::NoMiddleware},
        {NodeManager::Middleware::ZMQ, Port::Middleware::Zmq},
        {NodeManager::Middleware::RTI, Port::Middleware::Rti},
        {NodeManager::Middleware::OSPL, Port::Middleware::Ospl},
        {NodeManager::Middleware::QPID, Port::Middleware::Qpid},
        {NodeManager::Middleware::TAO, Port::Middleware::Tao}});
    if(mw_map.count(middleware)){
        return mw_map.at(middleware);
    }
    return Port::Middleware::NoMiddleware;
}

NodeManager::Middleware Port::TranslateInternalMiddleware(const Port::Middleware middleware){
    const static std::map<Port::Middleware, NodeManager::Middleware> mw_map({
        {Port::Middleware::NoMiddleware, NodeManager::Middleware::NO_MIDDLEWARE},
        {Port::Middleware::Zmq, NodeManager::Middleware::ZMQ},
        {Port::Middleware::Rti, NodeManager::Middleware::RTI},
        {Port::Middleware::Ospl, NodeManager::Middleware::OSPL},
        {Port::Middleware::Qpid, NodeManager::Middleware::QPID},
        {Port::Middleware::Tao, NodeManager::Middleware::TAO}});
    if(mw_map.count(middleware)){
        return mw_map.at(middleware);
    }
    return NodeManager::Middleware::NO_MIDDLEWARE;
}

const std::vector<std::reference_wrapper<Port> > Port::GetConnectedPorts() const{
    std::vector<std::reference_wrapper<Port> > ports;

    for(auto port_id : GetInternalConnectedPortIds()){
        try{
            auto& port = GetExperiment().GetPort(port_id);
            ports.emplace_back(port);
        }catch(const std::exception& ex){

        }
    }

    for(auto port_id : GetExternalConnectedPortIds()){
        try{
            //Get the Port,
            auto& port = GetExperiment().GetPort(port_id);
            ports.emplace_back(port);
            continue;
        }catch(const std::exception& ex){
        
        }
        
        //Port is an ExternalModelDelegate
        try{
            const auto& external_port_label = GetExperiment().GetExternalPortLabel(port_id);
            auto external_ports = GetEnvironment().GetExternalProducerPorts(external_port_label);
            ports.insert(ports.end(), external_ports.begin(), external_ports.end());
        }catch(const std::exception& ex){

        }
    }
    return ports;
}

bool Port::IsBlackbox() const{
    return (blackbox_type_ == Port::BlackboxType::PubSub) || (blackbox_type_ == Port::BlackboxType::ReqRep);
}