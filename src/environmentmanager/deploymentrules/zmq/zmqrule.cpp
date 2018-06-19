#include "zmqrule.h"
#include <iostream>
Zmq::DeploymentRule::DeploymentRule(Environment& environment) : ::DeploymentRule(::DeploymentRule::MiddlewareType::ZMQ, environment){
}

void Zmq::DeploymentRule::ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port){


    //find deployed hardware node
    switch(event_port.kind()){
        case NodeManager::Port::PUBLISHER:
        case NodeManager::Port::SUBSCRIBER:{
            //set publisher_address
            auto publisher_address_attr = event_port.add_attributes();
            auto publisher_address_attr_info = publisher_address_attr->mutable_info();
            publisher_address_attr_info->set_name("publisher_address");
            publisher_address_attr->set_kind(NodeManager::Attribute::STRINGLIST);

            auto publisher_addresses = environment_.GetPublisherAddress(message.experiment_id(), event_port);
            if(event_port.kind() == NodeManager::Port::PUBLISHER){
                publisher_address_attr->add_s(publisher_addresses.at(0));

            }
            else{
                for(auto id : publisher_addresses){
                    publisher_address_attr->add_s(id);
                }
            }
            break;
        }

        case NodeManager::Port::REQUESTER:
        case NodeManager::Port::REPLIER:{
            auto server_address_attr = event_port.add_attributes();
            auto server_address_attr_info = server_address_attr->mutable_info();
            server_address_attr_info->set_name("server_address");
            server_address_attr->set_kind(NodeManager::Attribute::STRING);

            auto connected_address = environment_.GetPublisherAddress(message.experiment_id(), event_port);
            server_address_attr->add_s(connected_address.at(0));
            break;
        }

        default:{
            break;
        }
    }
}
void Zmq::DeploymentRule::TerminateEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port){

}