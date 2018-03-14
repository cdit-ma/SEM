#include "zmqrule.h"

Zmq::DeploymentRule::DeploymentRule(Environment& environment) : ::DeploymentRule(::DeploymentRule::MiddlewareType::ZMQ){
    environment_ = environment;
}

void Zmq::DeploymentRule::ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::EventPort* event_port){

    //set port number
    auto port_number_attr = event_port->add_attributes();
    port_number_attr->set_name("port_number");
    port_number_attr->set_kind(NodeManager::Attribute::STRING);
    port_number_attr->set_s("1234");

    //set addr
    auto addr_attr = event_port->add_attributes();
    addr_attr->set_name("address");
    addr_attr->set_kind(NodeManager::Attribute::STRING);
    addr_attr->set_s("192.168.111.230");


}
void Zmq::DeploymentRule::TerminateEventPort(const NodeManager::ControlMessage& message, NodeManager::EventPort& event_port){

}
