#include "ddsrule.h"

Dds::DeploymentRule::DeploymentRule(Environment& environment) : ::DeploymentRule(::DeploymentRule::MiddlewareType::ZMQ, environment){
}

void Dds::DeploymentRule::ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::EventPort& event_port){
    //Set domain
    auto domain_pb = event_port.add_attributes();
    auto domain_info_pb = domain_pb->mutable_info();
    domain_info_pb->set_name("domain_id");
    domain_pb->set_kind(NodeManager::Attribute::INTEGER);
    domain_pb->set_i(0);

    //set and add topics to environment
    //look up connected ports and fill in topic if we dont have one?

}
void Dds::DeploymentRule::TerminateEventPort(const NodeManager::ControlMessage& message, NodeManager::EventPort& event_port){

}
