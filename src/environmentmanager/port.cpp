#include "node.h"
#include "component.h"
#include "port.h"
#include "environment.h"

using namespace EnvironmentManager;

Port::Port(Environment& environment, Component& parent, const NodeManager::Port& port) :
            environment_(environment), component_(parent){

    id_ = port.info().id();
    name_ = port.info().name();
    kind_ = ProtoPortKindToInternal(port.kind());
    middleware_ = ProtoPortMiddlewareToInternal(port.middleware());

    //If we're a zmq publisher or zmq replier port, assign a publisher port
    if(middleware_ == EnvironmentManager::Port::Middleware::Zmq){
        
        if(kind_ == EnvironmentManager::Port::Kind::Publisher || kind_ == EnvironmentManager::Port::Kind::Replier){
            SetPublisherPort(environment_.GetPort(component_.GetNode().GetIp()));
            std::string endpoint = "tcp://" + GetNode().GetIp() + ":" + GetPublisherPort();
            GetExperiment().AddZmqEndpoint(id_, endpoint);
        }
    }
    else if(middleware_ == EnvironmentManager::Port::Middleware::Tao){
        if(!GetNode().HasOrbPort()){
            GetNode().SetOrbPort(environment_.GetPort(GetNode().GetIp()));
        }
        SetPublisherPort(GetNode().GetOrbPort());
        SetTopic(GetName() + "_" + GetId());
    }

    std::string type;
    for(const auto& ns : port.info().namespaces()){
        type += ns + "::";
    }

    type += port.info().type();
    SetType(type);
    
    for(int a = 0; a < port.attributes_size(); a++){
        auto attribute = port.attributes(a);
        if(attribute.info().name() == "topic"){
            SetTopic(attribute.s(0));
            GetExperiment().AddTopic(GetTopic());
            break;
        }
    }

    for(int k = 0; k < port.connected_ports_size(); k++){
        auto connected_id = port.connected_ports(k);
        AddConnectedPortId(connected_id);
        //connection_map_[connected_id].push_back(GetId());
        GetExperiment().AddConnection(connected_id, id_);
    }
}

Port::~Port(){
    if(middleware_ == EnvironmentManager::Port::Middleware::Zmq && 
        (kind_ == Kind::Publisher || kind_ == Kind::Replier)){
        environment_.FreePort(GetNode().GetId(), publisher_port_);
        GetExperiment().RemoveZmqEndpoint(id_);
    }
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

Component& Port::GetComponent() const{
    return component_;
}
Node& Port::GetNode() const{
    return component_.GetNode();
}
Experiment& Port::GetExperiment() const{
    return GetNode().GetExperiment();
}

void Port::SetType(const std::string& type){
    type_ = type;
}

void Port::SetTopic(const std::string& topic_name){
    topic_name_ = topic_name;
}

void Port::SetPublisherPort(const std::string& publisher_port){
    publisher_port_ = publisher_port;
}

std::string Port::GetPublisherPort() const{
    return publisher_port_;
}

std::string Port::GetTopic() const{
    return topic_name_;
}

void Port::AddEndpoint(const std::string& endpoint){
    endpoints_.insert(endpoint);
}

void Port::RemoveEndpoint(const std::string& endpoint){
    endpoints_.erase(endpoints_.find(endpoint));
}

void Port::AddConnectedPortId(const std::string& port_id){
    connected_port_ids_.insert(port_id);
}

void Port::AddExternalConnectedPortId(const std::string& port_id){
    connected_external_port_ids_.insert(port_id);
}

bool Port::IsConnectedTo(const std::string& port_id) const{
    return (connected_port_ids_.find(port_id) != connected_port_ids_.end()) || (connected_external_port_ids_.find(port_id) != connected_external_port_ids_.end());
}

void Port::SetDirty(){
    component_.SetDirty();
    dirty_ = true;
}

bool Port::IsDirty() const{
    return dirty_;
}

NodeManager::Port* Port::GetUpdate(){
    NodeManager::Port* port;

    if(dirty_){
        port = new NodeManager::Port();
        port->mutable_info()->set_name(name_);
        port->mutable_info()->set_id(id_);
        port->mutable_info()->set_type(type_);

        port->set_kind(InternalPortKindToProto(kind_));
        port->set_middleware(InternalPortMiddlewareToProto(middleware_));

        for(const auto& connected_port : connected_port_ids_){
            port->add_connected_ports(connected_port);
        }

        for(const auto& external_port : connected_external_port_ids_){
            port->add_connected_ports(external_port);
        }

        // for(const auto& attribute : attributes_){
        //     std::cout << "add attrs" << std::endl;
        // }

        dirty_ = false;
    }
    return port;
}

Port::Kind Port::ProtoPortKindToInternal(NodeManager::Port::Kind kind){
    switch(kind){
        case NodeManager::Port::NO_KIND:{
            return Port::Kind::NoKind;
        }
        case NodeManager::Port::PERIODIC:{
            return Port::Kind::Periodic;
        }
        case NodeManager::Port::PUBLISHER:{
            return Port::Kind::Publisher;
        }
        case NodeManager::Port::SUBSCRIBER:{
            return Port::Kind::Subscriber;
        }
        case NodeManager::Port::REQUESTER:{
            return Port::Kind::Requester;
        }
        case NodeManager::Port::REPLIER:{
            return Port::Kind::Replier;
        }
        default:{
            return Port::Kind::NoKind;
        }
    }
    return Port::Kind::NoKind;
}

NodeManager::Port::Kind Port::InternalPortKindToProto(Port::Kind kind){
    switch(kind){
        case Port::Kind::NoKind:{
            return NodeManager::Port::NO_KIND;
        }
        case Port::Kind::Periodic:{
            return NodeManager::Port::PERIODIC;
        }
        case Port::Kind::Publisher:{
            return NodeManager::Port::PUBLISHER;
        }
        case Port::Kind::Subscriber:{
            return NodeManager::Port::SUBSCRIBER;
        }
        case Port::Kind::Requester:{
            return NodeManager::Port::REQUESTER;
        }
        case Port::Kind::Replier:{
            return NodeManager::Port::REPLIER;
        }
        default:{
            return NodeManager::Port::NO_KIND;
        }
    }
    return NodeManager::Port::NO_KIND;
}

Port::Middleware Port::ProtoPortMiddlewareToInternal(NodeManager::Middleware middleware){
    switch(middleware){
        case NodeManager::Middleware::NO_MIDDLEWARE:{
            return Port::Middleware::NoMiddleware;
            break;
        }
        case NodeManager::Middleware::ZMQ:{
            return Port::Middleware::Zmq;
            break;
        }
        case NodeManager::Middleware::RTI:{
            return Port::Middleware::Rti;
            break;
        }
        case NodeManager::Middleware::OSPL:{
            return Port::Middleware::Ospl;
            break;
        }
        case NodeManager::Middleware::QPID:{
            return Port::Middleware::Qpid;
            break;
        }
        case NodeManager::Middleware::TAO:{
            return Port::Middleware::Tao;
            break;
        }
        default:{
            return Port::Middleware::NoMiddleware;
        }
    }
    return Port::Middleware::NoMiddleware;
}

NodeManager::Middleware Port::InternalPortMiddlewareToProto(Port::Middleware middleware){
    switch(middleware){
        case Port::Middleware::NoMiddleware:{
            return NodeManager::Middleware::NO_MIDDLEWARE;
            break;
        }
        case Port::Middleware::Zmq:{
            return NodeManager::Middleware::ZMQ;
            break;
        }
        case Port::Middleware::Rti:{
            return NodeManager::Middleware::RTI;
            break;
        }
        case Port::Middleware::Ospl:{
            return NodeManager::Middleware::OSPL;
            break;
        }
        case Port::Middleware::Qpid:{
            return NodeManager::Middleware::QPID;
            break;
        }
        case Port::Middleware::Tao:{
            return NodeManager::Middleware::TAO;
            break;
        }
        default:{
            return NodeManager::Middleware::NO_MIDDLEWARE;
        }
    }
    return NodeManager::Middleware::NO_MIDDLEWARE;
}