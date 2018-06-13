#include "taorule.h"
#include <iostream>

Tao::DeploymentRule::DeploymentRule(Environment& environment) : ::DeploymentRule(::DeploymentRule::MiddlewareType::TAO, environment){
}

void Tao::DeploymentRule::ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port){
    const auto& endpoint = std::string("192.168.111.90:50005");
    const auto& orb_endpoint = std::string("iiop:// " + endpoint);
    const auto& server_name = std::string("TEST_SERVER");
    const auto& server_address = std::string("corbaloc:iiop:" + endpoint + "/" + server_name);
    {
        //TODO: Need Unique Port per HardwareNode per experiment for both Requester/Replier
        auto orb_attr = event_port.add_attributes();
        auto orb_info = orb_attr->mutable_info();
        orb_info->set_name("orb_endpoint");
        orb_attr->set_kind(NodeManager::Attribute::STRING);
        orb_attr->add_s(orb_endpoint);
    }

    {
        //TODO: Need Unique Label within the Port
        auto server_name_attr = event_port.add_attributes();
        auto server_name_info = server_name_attr->mutable_info();
        server_name_info->set_name("server_name");
        server_name_attr->set_kind(NodeManager::Attribute::STRING);
        server_name_attr->add_s(server_name);
    }

    if(event_port.kind() == NodeManager::Port::REQUESTER){
        //TODO: Need Unique Port per HardwareNode per experiment for both Requester/Replier
        auto server_addr_attr = event_port.add_attributes();
        auto server_addr_info = server_addr_attr->mutable_info();
        server_addr_info->set_name("server_address");
        server_addr_attr->set_kind(NodeManager::Attribute::STRING);
        server_addr_attr->add_s(server_address);
    }
}
void Tao::DeploymentRule::TerminateEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port){

}