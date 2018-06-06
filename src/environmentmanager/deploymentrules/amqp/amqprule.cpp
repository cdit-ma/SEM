#include "amqprule.h"

Amqp::DeploymentRule::DeploymentRule(Environment& environment) : ::DeploymentRule(::DeploymentRule::MiddlewareType::AMQP, environment){
}

void Amqp::DeploymentRule::ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port){
    //Set broker address
    auto broker_pb = event_port.add_attributes();
    auto broker_info_pb = broker_pb->mutable_info();
    broker_info_pb->set_name("broker");
    broker_pb->set_kind(NodeManager::Attribute::STRING);
    broker_pb->add_s(environment_.GetAmqpBrokerAddress());

    //set and add topics to environment
    bool has_topic = false;
    for(int i = 0; i < event_port.attributes_size(); i++){
        auto attribute = event_port.attributes(i);
        if(attribute.info().name() == "topic_name" && !attribute.s(0).empty()){
            has_topic = true;
            auto topic_conflicts = environment_.CheckTopic(message.experiment_id(), attribute.info().name());
            if(!topic_conflicts.empty()){
                //TODO: Do something with topic conflicts. Raise warning on experiment startup.
            }
            break;
        }
    }

    if(!has_topic){
        throw std::invalid_argument(event_port.port_guid() + " has no topic.");
    }
}
void Amqp::DeploymentRule::TerminateEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port){

}
