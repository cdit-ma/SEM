#include "zmqport.h"
#include "../environment.h"
#include "../node.h"
#include <re_common/proto/controlmessage/helper.h>

using namespace EnvironmentManager::zmq;

Port::Port(Component& parent, const NodeManager::Port& port) :
    ::EnvironmentManager::Port(parent, port){

    if(kind_ == EnvironmentManager::Port::Kind::Publisher || kind_ == EnvironmentManager::Port::Kind::Replier){
        ip_ = GetNode().GetIp();
        producer_port_ = GetEnvironment().GetPort(ip_);
    }
    producer_endpoint_ = "tcp://" + ip_ + ":" + producer_port_;
}

Port::Port(::EnvironmentManager::Experiment& parent, const NodeManager::ExternalPort& port) :
    ::EnvironmentManager::Port(parent, port){
    if(GetBlackboxType() == BlackboxType::PubSub){
        producer_endpoint_ = NodeManager::GetAttribute(port.attributes(), "publisher_address").s(0);
    }
    else if(GetBlackboxType() == BlackboxType::ReqRep){
        producer_endpoint_ = NodeManager::GetAttribute(port.attributes(), "server_address").s(0);
    }
}

Port::~Port(){
    if(!IsBlackbox()){
        if(kind_ == Kind::Publisher || kind_ == Kind::Replier){
            GetEnvironment().FreePort(ip_,  GetProducerPort());
        }
    }
}

std::string Port::GetProducerPort() const{
    return producer_port_;
}
std::string Port::GetProducerEndpoint() const{
    return producer_endpoint_;
}

void Port::FillPortPb(NodeManager::Port& port_pb){
    const auto& port_kind = GetKind();
    
    auto attrs = port_pb.mutable_attributes();


    if(port_kind == Kind::Publisher){
        NodeManager::SetStringAttribute(attrs, "publisher_address", GetProducerEndpoint());
    }
    else if(port_kind == Kind::Replier){
        NodeManager::SetStringAttribute(attrs, "server_address", GetProducerEndpoint());
    }
    else if(port_kind == Kind::Subscriber){
        std::vector<std::string> publisher_endpoints;
        //Connect all Internal ports
        for(const auto& port_ref : GetConnectedPorts()){
            const auto& port = port_ref.get();
            if(port.GetMiddleware() == ::EnvironmentManager::Port::Middleware::Zmq){
                const auto& zmq_port = (const Port&)port;
                publisher_endpoints.push_back(zmq_port.GetProducerEndpoint());
            }
        }
        NodeManager::SetStringListAttribute(attrs, "publisher_addresses", publisher_endpoints);
    }
    else if(port_kind == Kind::Requester){
        //Connect all Internal ports
        std::vector<std::string> replier_endpoints;
        for(const auto& port_ref : GetConnectedPorts()){
            const auto& port = port_ref.get();
            if(port.GetMiddleware() == ::EnvironmentManager::Port::Middleware::Zmq){
                const auto& zmq_port = (const Port&)port;
                replier_endpoints.push_back(zmq_port.GetProducerEndpoint());
            }
        }
        NodeManager::SetStringListAttribute(attrs, "server_address", replier_endpoints);
    }
}
