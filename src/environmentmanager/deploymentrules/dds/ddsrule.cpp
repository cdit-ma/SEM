#include "ddsrule.h"

Dds::DeploymentRule::DeploymentRule(Environment& environment) : ::DeploymentRule(::DeploymentRule::MiddlewareType::DDS, environment){
}

void Dds::DeploymentRule::ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::EventPort& event_port){
    std::string model_name = message.model_name();
    //Set domain
    auto domain_pb = event_port.add_attributes();
    auto domain_info_pb = domain_pb->mutable_info();
    domain_info_pb->set_name("domain_id");
    domain_pb->set_kind(NodeManager::Attribute::INTEGER);
    domain_pb->set_i(0);

    //set and add topics to environment
    //look up connected ports and fill in topic if we dont have one

    
    bool has_topic = false;
    for(int i = 0; i < event_port.attributes_size(); i++){
        auto attribute = event_port.attributes(i);
        if(attribute.info().name() == "topic_name" && !attribute.s(0).empty()){
            has_topic = true;
            break;
        }
    }

    if(!has_topic){
        throw std::invalid_argument(event_port.port_guid() + " has no topic.");
    }
}
void Dds::DeploymentRule::TerminateEventPort(const NodeManager::ControlMessage& message, NodeManager::EventPort& event_port){

}
