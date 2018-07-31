#include "taoport.h"
#include "../environment.h"
#include "../component.h"
#include "../node.h"
#include <re_common/proto/controlmessage/helper.h>

using namespace EnvironmentManager::tao;


Port::Port(Component& parent, const NodeManager::Port& port):
    ::EnvironmentManager::Port(parent, port){

    //Assign port to this
    auto& node = GetNode();
    const auto& orb_port = node.AssignOrbPort();

    //Set Orb endpoint
    orb_endpoint_ = "iiop://" + node.GetIp() + ":" + orb_port;

    //Set the server name
    const auto& server_name = GetName() + "_" + GetId();
    server_name_.push_back(server_name);

    //Set the naming service endpoint
    naming_service_endpoint_ = "corbaloc:iiop:" + GetEnvironment().GetTaoNamingServiceAddress();
}

Port::Port(::EnvironmentManager::Experiment& parent, const NodeManager::ExternalPort& port):
    ::EnvironmentManager::Port(parent, port){
}

Port::~Port(){

}

const std::vector<std::string>& Port::GetServerName() const{
    return server_name_;
}

const std::string& Port::GetNamingServiceEndpoint() const{
    return naming_service_endpoint_;
}

const std::string& Port::GetOrbEndpoint() const{
    return orb_endpoint_;
}

void Port::FillPortPb(NodeManager::Port& port_pb){
    auto attrs = port_pb.mutable_attributes();

    if(!IsBlackbox()){
        NodeManager::SetStringAttribute(attrs, "orb_endpoint", GetOrbEndpoint());
    }

    //If we're a replier, we can set our server name(topic) and naming service endpoint based on internal information.
    if(GetKind() == Kind::Replier){
        //Set the Server Name Attribute
        NodeManager::SetStringListAttribute(attrs, "server_name", GetServerName());
        NodeManager::SetStringAttribute(attrs, "naming_service_endpoint", GetNamingServiceEndpoint());
    }
    
    else if(GetKind() == Kind::Requester){
        //Connect all Internal ports
        for(const auto& connected_port_ref : GetConnectedPorts()){
            const auto& connected_port = connected_port_ref.get();
            const auto& tao_port = (const Port&) connected_port;

            NodeManager::SetStringListAttribute(attrs, "server_name", tao_port.GetServerName());
            NodeManager::SetStringAttribute(attrs, "naming_service_endpoint", tao_port.GetNamingServiceEndpoint());
            break;
        }
    }
}