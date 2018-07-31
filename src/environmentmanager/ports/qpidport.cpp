#include "qpidport.h"
#include "../environment.h"
#include "../experiment.h"


using namespace EnvironmentManager::qpid;

Port::Port(::EnvironmentManager::Component& parent, const NodeManager::Port& port) :
    ::EnvironmentManager::Port(parent, port){

    broker_address_ = GetEnvironment().GetAmqpBrokerAddress();

    for(const auto& attribute : port.attributes()){
        if(attribute.info().name() == "topic_name"){
            topic_name_ = attribute.s(0);
        }
    }
}

Port::Port(::EnvironmentManager::Experiment& parent, const NodeManager::ExternalPort& port) :
    ::EnvironmentManager::Port(parent, port){

}

Port::~Port(){

}



std::string Port::GetTopic() const{
    return topic_name_;
}

std::string Port::GetAmqpBrokerAddress() const{
    return broker_address_;
}

void Port::FillPortPb(NodeManager::Port& port_pb){
    auto broker_pb = port_pb.add_attributes();
    broker_pb->mutable_info()->set_name("broker");
    broker_pb->set_kind(NodeManager::Attribute::STRING);
    broker_pb->add_s(GetEnvironment().GetAmqpBrokerAddress());
    
    FillTopicPb(port_pb);
}
