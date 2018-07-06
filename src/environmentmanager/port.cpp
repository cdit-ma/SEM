#include "port.h"
#include "node.h"
#include "component.h"
#include "environment.h"
#include "experiment.h"
#include "attribute.h"

using namespace EnvironmentManager;

Port::Port(Environment& environment, Component& parent, const NodeManager::Port& port) :
            environment_(environment), component_(parent){

    id_ = port.info().id();
    name_ = port.info().name();
    kind_ = ProtoPortKindToInternal(port.kind());
    middleware_ = ProtoPortMiddlewareToInternal(port.middleware());

    for(int i = 0; i < port.attributes_size(); i++){
        AddAttribute(port.attributes(i));
    }

    //If we're a zmq publisher or zmq replier port, assign a publisher port
    if(middleware_ == EnvironmentManager::Port::Middleware::Zmq){
        
        if(kind_ == EnvironmentManager::Port::Kind::Publisher || kind_ == EnvironmentManager::Port::Kind::Replier){
            SetPublisherPort(environment_.GetPort(component_.GetNode().GetIp()));
            std::string endpoint = "tcp://" + GetNode().GetIp() + ":" + GetPublisherPort();
            GetExperiment().AddZmqEndpoint(id_, endpoint);
            endpoints_.insert(endpoint);

            //if this port is connected to any external ports, update them to have this endpoint
            for(int i = 0; i < port.connected_external_ports_size(); i++){
                external_ = true;
                GetExperiment().AddExternalEndpoint(port.connected_external_ports(i), endpoint);
            }
        }
    }
    if(middleware_ == EnvironmentManager::Port::Middleware::Tao){
        SetPublisherPort(GetNode().GetOrbPort());
        std::string topic = GetName() + "_" + GetId();
        SetTopic(topic);

        if(kind_ == Kind::Replier){
            std::string endpoint = "corbaloc:iiop:" + GetNode().GetIp() + ":" + GetPublisherPort() + "/" + topic;
            GetExperiment().AddTaoEndpoint(id_, endpoint);
            endpoints_.insert(endpoint);

            //if this port is connected to any external ports, update them to have this endpoint
            for(int i = 0; i < port.connected_external_ports_size(); i++){
                external_ = true;
                GetExperiment().AddExternalEndpoint(port.connected_external_ports(i), endpoint);
            }
        }
    }

    //Append to the list of namespaces
    for(const auto& ns : port.info().namespaces()){
        namespaces_.emplace_back(ns);
    }

    //Use the exact type
    SetType(port.info().type());
    
    if(middleware_ != Middleware::Tao){
        for(int a = 0; a < port.attributes_size(); a++){
            auto attribute = port.attributes(a);
            if(attribute.info().name() == "topic"){
                SetTopic(attribute.s(0));
                GetExperiment().AddTopic(GetTopic());
                break;
            }
        }
    }

    for(int i = 0; i < port.connected_ports_size(); i++){
        auto connected_id = port.connected_ports(i);
        AddConnectedPortId(connected_id);
        GetExperiment().AddConnection(connected_id, id_);
    }
    for(int i = 0; i < port.connected_external_ports_size(); i++){
        auto connected_id = port.connected_external_ports(i);
        AddExternalConnectedPortId(connected_id);
        GetExperiment().AddConnection(connected_id, id_);
    }
}

Port::~Port(){
    //Dont need to handle freeing tao ports, this is done at node level.

    if(middleware_ == Middleware::Zmq && 
        (kind_ == Kind::Publisher || kind_ == Kind::Replier)){
        environment_.FreePort(GetNode().GetIp(), publisher_port_);
        GetExperiment().RemoveZmqEndpoint(id_);
    }

    if(external_){
        GetExperiment().RemoveExternalEndpoint(id_);
    }
}

void Port::ConfigureConnections(){

    if(middleware_ == Middleware::Zmq){
        if(kind_ == Kind::Requester || kind_ == Kind::Subscriber){
            const auto& endpoint_map = GetExperiment().GetZmqEndpointMap();
            for(const auto& connected_port_id : connected_port_ids_){
                if(endpoint_map.count(connected_port_id)){
                    auto endpoint = endpoint_map.at(connected_port_id);
                    AddConnectedEndpoint(endpoint);
                }
            }
        }
    }

    if(middleware_ == Middleware::Tao){
        if(kind_ == Kind::Requester){
            const auto& endpoint_map = GetExperiment().GetTaoEndpointMap();
            for(const auto& connected_port_id : connected_port_ids_){
                if(endpoint_map.count(connected_port_id)){
                    auto endpoint = endpoint_map.at(connected_port_id);
                    AddConnectedEndpoint(endpoint);
                }
            }
        }
    }

    for(const auto& external_connected_port_id : connected_external_port_ids_){
        auto external_port_label = GetExperiment().GetPublicEventPortName(external_connected_port_id);
        if(environment_.HasPublicEventPort(external_port_label)){
            AddExternalConnectedEndpoint(environment_.GetPublicEventPortEndpoint(external_port_label));
        }
    }
}

void Port::AddAttribute(const NodeManager::Attribute& attribute){
    attributes_.insert(std::make_pair(attribute.info().id(), 
            std::unique_ptr<EnvironmentManager::Attribute>(
                new EnvironmentManager::Attribute(attribute))));
}

void Port::UpdateExternalEndpoints(){
    if(kind_ == Kind::Requester || kind_ == Kind::Subscriber){
        //Clear out our external endpoints
        external_endpoints_.clear();

        //Repopulate them from the environment
        for(const auto& external_connected_port_id : connected_external_port_ids_){
            auto external_port_label = GetExperiment().GetPublicEventPortName(external_connected_port_id);
            if(environment_.HasPublicEventPort(external_port_label)){
                SetDirty();
                AddExternalConnectedEndpoint(environment_.GetPublicEventPortEndpoint(external_port_label));
            }
        }
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

void Port::AddConnectedEndpoint(const std::string& endpoint){
    endpoints_.insert(endpoint);
}

void Port::RemoveConnectedEndpoint(const std::string& endpoint){
    endpoints_.erase(endpoints_.find(endpoint));
}

void Port::AddExternalConnectedEndpoint(const std::string& endpoint){
    external_endpoints_.insert(endpoint);
}

void Port::RemoveExternalConnectedEndpoint(const std::string& endpoint){
    external_endpoints_.erase(endpoints_.find(endpoint));
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
        port = GetProto();
        dirty_ = false;
    }
    return port;
}

NodeManager::Port* Port::GetProto(){
    NodeManager::Port* port;

    port = new NodeManager::Port();
    port->mutable_info()->set_name(name_);
    port->mutable_info()->set_id(id_);
    port->mutable_info()->set_type(type_);
    for(const auto& ns : namespaces_){
        port->mutable_info()->add_namespaces(ns);
    }
    

    port->set_kind(InternalPortKindToProto(kind_));
    port->set_middleware(InternalPortMiddlewareToProto(middleware_));

    for(const auto& connected_port : connected_port_ids_){
        port->add_connected_ports(connected_port);
    }

    for(const auto& external_port : connected_external_port_ids_){
        port->add_connected_external_ports(external_port);
    }

    switch(middleware_){
        case Middleware::Zmq:{
            FillZmqProto(port);
            break;
        }
        case Middleware::Rti:
        case Middleware::Ospl:{
            FillDdsProto(port);
            break;
        }
        case Middleware::Qpid:{
            FillQpidProto(port);
            break;
        }
        case Middleware::Tao:{
            FillTaoProto(port);
            break;
        }
        default:{
            break;
        }
    }

    for(const auto& attribute : attributes_){
        port->mutable_attributes()->AddAllocated(attribute.second->GetProto());
    }

    return port;
}

void Port::FillZmqProto(NodeManager::Port* port){
    switch(kind_){
        case Kind::Publisher:
        case Kind::Subscriber:{
            auto publisher_address_attr = port->add_attributes();
            auto publisher_address_attr_info = publisher_address_attr->mutable_info();
            publisher_address_attr_info->set_name("publisher_address");
            publisher_address_attr->set_kind(NodeManager::Attribute::STRINGLIST);

            if(kind_ == Kind::Publisher){
                publisher_address_attr->add_s(*(endpoints_.begin()));
            }
            else{
                for(auto endpoint : endpoints_){
                    publisher_address_attr->add_s(endpoint);
                }
                for(auto endpoint : external_endpoints_){
                    publisher_address_attr->add_s(endpoint);
                }
            }
            break;
        }
        case Kind::Requester:
        case Kind::Replier:{
            auto server_address_attr = port->add_attributes();
            auto server_address_attr_info = server_address_attr->mutable_info();
            server_address_attr_info->set_name("server_address");
            server_address_attr->set_kind(NodeManager::Attribute::STRING);

            if(!endpoints_.empty()){
                server_address_attr->add_s(*(endpoints_.begin()));
            }
            break;
        }
        default:{
            break;
        }
    }
}

void Port::FillDdsProto(NodeManager::Port* port){
    //Set domain
    auto domain_pb = port->add_attributes();
    auto domain_info_pb = domain_pb->mutable_info();
    domain_info_pb->set_name("domain_id");
    domain_pb->set_kind(NodeManager::Attribute::INTEGER);
    domain_pb->set_i(0);

    auto topic_pb = port->add_attributes();
    auto topic_info_pb = topic_pb->mutable_info();
    topic_info_pb->set_name("topic_name");
    topic_pb->set_kind(NodeManager::Attribute::STRING);
    topic_pb->add_s(topic_name_);
}

void Port::FillQpidProto(NodeManager::Port* port){
    //Set broker address
    auto broker_pb = port->add_attributes();
    auto broker_info_pb = broker_pb->mutable_info();
    broker_info_pb->set_name("broker");
    broker_pb->set_kind(NodeManager::Attribute::STRING);
    broker_pb->add_s(environment_.GetAmqpBrokerAddress());

    //set and add topics to environment
    auto topic_pb = port->add_attributes();
    auto topic_info_pb = topic_pb->mutable_info();
    topic_info_pb->set_name("topic_name");
    topic_pb->set_kind(NodeManager::Attribute::STRING);
    topic_pb->add_s(topic_name_);
}

void Port::FillTaoProto(NodeManager::Port* port){
    {
        auto server_name_attr = port->add_attributes();
        auto server_name_info = server_name_attr->mutable_info();
        server_name_info->set_name("server_name");
        server_name_attr->set_kind(NodeManager::Attribute::STRING);
        server_name_attr->add_s(topic_name_);
    }

    const auto& orb_endpoint = GetNode().GetIp() + ":" + GetNode().GetOrbPort();
    {
        //TODO: Need Unique Port per HardwareNode per experiment for both Requester/Replier
        auto orb_attr = port->add_attributes();
        auto orb_info = orb_attr->mutable_info();
        orb_info->set_name("orb_endpoint");
        orb_attr->set_kind(NodeManager::Attribute::STRING);
        orb_attr->add_s("iiop://" + orb_endpoint);
    }

    if(kind_ == Kind::Requester){
        //TODO: Need Unique Port per HardwareNode per experiment for both Requester/Replier
        auto server_addr_attr = port->add_attributes();
        auto server_addr_info = server_addr_attr->mutable_info();
        server_addr_info->set_name("server_address");
        server_addr_attr->set_kind(NodeManager::Attribute::STRING);
        if(!endpoints_.empty()){
            server_addr_attr->add_s(*(endpoints_.begin()));
        }
        else if(!external_endpoints_.empty()){
            server_addr_attr->add_s(*(external_endpoints_.begin()));

        }
    }
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