#include "zmqrule.h"

Zmq::DeploymentRule::DeploymentRule(Environment& environment) : ::DeploymentRule(::DeploymentRule::MiddlewareType::ZMQ, environment){
}

void Zmq::DeploymentRule::ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::EventPort& event_port){

    //set publisher_address
    auto publisher_address_attr = event_port.add_attributes();
    auto publisher_address_attr_info = publisher_address_attr->mutable_info();
    publisher_address_attr_info->set_name("publisher_address");
    publisher_address_attr->set_kind(NodeManager::Attribute::STRINGLIST);

    //Get hardware addr and 
    publisher_address_attr->add_s("");
}
void Zmq::DeploymentRule::TerminateEventPort(const NodeManager::ControlMessage& message, NodeManager::EventPort& event_port){

}
