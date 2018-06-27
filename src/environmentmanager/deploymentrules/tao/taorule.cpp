#include "taorule.h"
#include <iostream>

Tao::DeploymentRule::DeploymentRule(Environment& environment) : ::DeploymentRule(::DeploymentRule::MiddlewareType::TAO, environment){
}

void Tao::DeploymentRule::ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port){
    //TODO: Ask environment manager for this
    const auto& server_name = environment_.GetTaoServerName(message.experiment_id(), event_port);
    {
        auto server_name_attr = event_port.add_attributes();
        auto server_name_info = server_name_attr->mutable_info();
        server_name_info->set_name("server_name");
        server_name_attr->set_kind(NodeManager::Attribute::STRING);
        server_name_attr->add_s(server_name);
    }

    const auto& endpoint = environment_.GetOrbEndpoint(message.experiment_id(), event_port.info().id());
    {
        //TODO: Need Unique Port per HardwareNode per experiment for both Requester/Replier
        auto orb_attr = event_port.add_attributes();
        auto orb_info = orb_attr->mutable_info();
        orb_info->set_name("orb_endpoint");
        orb_attr->set_kind(NodeManager::Attribute::STRING);
        orb_attr->add_s("iiop://" + endpoint);
    }


    if(event_port.kind() == NodeManager::Port::REQUESTER){
        const auto& server_address = environment_.GetTaoReplierServerAddress(message.experiment_id(), event_port);
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