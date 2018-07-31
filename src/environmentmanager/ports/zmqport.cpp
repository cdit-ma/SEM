#include "zmqport.h"
#include "../environment.h"
#include "../node.h"

using namespace EnvironmentManager::zmq;

Port::Port(Component& parent, const NodeManager::Port& port) :
    ::EnvironmentManager::Port(parent, port){

    if(kind_ == EnvironmentManager::Port::Kind::Publisher || kind_ == EnvironmentManager::Port::Kind::Replier){
        ip_ = GetNode().GetIp();
        producer_port_ = GetEnvironment().GetPort(ip_);
    }

    for(const auto& attribute : port.attributes()){
        if(attribute.info().name() == "topic_name"){
            topic_name_ = attribute.s(0);
        }
    }
}

Port::Port(::EnvironmentManager::Experiment& parent, const NodeManager::ExternalPort& port) :
    ::EnvironmentManager::Port(parent, port){

}

Port::~Port(){

}

std::string Port::GetProducerPort() const{
    return producer_port_;
}
std::string Port::GetProducerEndpoint() const{
    return "tcp://" + ip_ + ":" + GetProducerPort();
}
std::string Port::GetTopic() const{
    return topic_name_;
}

void Port::FillPortPb(NodeManager::Port& port_pb){
    const auto& port_kind = GetKind();

    //Construct an attribute
    auto addr_pb = port_pb.add_attributes();

    if(port_kind == Kind::Publisher || port_kind == Kind::Subscriber){
        addr_pb->mutable_info()->set_name("publisher_address");
        addr_pb->set_kind(NodeManager::Attribute::STRINGLIST);
    }else if(port_kind == Kind::Requester || port_kind == Kind::Replier){
        addr_pb->mutable_info()->set_name("server_address");
        addr_pb->set_kind(NodeManager::Attribute::STRING);
    }
        
    if(port_kind == Kind::Publisher || port_kind == Kind::Replier){
        const auto& endpoint = GetProducerEndpoint();
        addr_pb->add_s(endpoint);
    }else if(port_kind == Kind::Subscriber || port_kind == Kind::Requester){
        //Connect all Internal ports
        for(const auto& port_ref : GetConnectedPorts()){
            const auto& port = port_ref.get();
            if(port.GetMiddleware() == ::EnvironmentManager::Port::Middleware::Zmq){
                const auto& zmq_port = (const Port&)port;
                const auto& endpoint = zmq_port.GetProducerEndpoint();
                addr_pb->add_s(endpoint);
            }
        }
    }
}
