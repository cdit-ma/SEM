#include "qpidport.h"
#include "../environment.h"
#include "../experiment.h"
#include <re_common/proto/controlmessage/helper.h>



using namespace EnvironmentManager::qpid;

Port::Port(::EnvironmentManager::Component& parent, const NodeManager::Port& port) :
    ::EnvironmentManager::Port(parent, port){

    broker_address_ = GetEnvironment().GetAmqpBrokerAddress();
    topic_name_ = NodeManager::GetAttribute(port.attributes(), "topic_name").s(0);
}

Port::Port(::EnvironmentManager::Experiment& parent, const NodeManager::ExternalPort& port) :
    ::EnvironmentManager::Port(parent, port){

}

Port::~Port(){

}

std::string Port::GetTopic() const{
    return topic_name_;
}

std::string Port::GetBrokerAddress() const{
    return broker_address_;
}

void Port::FillPortPb(NodeManager::Port& port_pb){
    auto attrs = port_pb.mutable_attributes();
    
    //TODO HANDLE DUPLICATE TOPICS??

    const auto& kind = GetKind();

    //If we're a replier, we can set our server name(topic) and naming service endpoint based on internal information.
    if(kind == Kind::Replier || kind == Kind::Publisher){
        NodeManager::SetStringAttribute(attrs, "broker", GetBrokerAddress());
        NodeManager::SetStringAttribute(attrs, "topic_name", GetTopic());
    }else if(kind == Kind::Requester || kind == Kind::Subscriber){
        std::set<std::string> brokers;
        std::set<std::string> topic_names;

        //Connect all Internal ports
        for(const auto& connected_port_ref : GetConnectedPorts()){
            const auto& connected_port = connected_port_ref.get();
            const auto& qpid_port = (const Port&) connected_port;

            brokers.insert(qpid_port.GetBrokerAddress());
            topic_names.insert(qpid_port.GetTopic());
        }

        if(topic_names.size() > 1){
            std::cerr << "* Experiment[" << GetExperiment().GetName() << "]: Has multiple topics connected to Port: '" << GetId() << "'" << std::endl;
        }

        if(brokers.size() > 1){
            std::cerr << "* Experiment[" << GetExperiment().GetName() << "]: Has multiple QPID Brokers connected to Port: '" << GetId() << "'" << std::endl;
        }

        NodeManager::SetStringAttribute(attrs, "broker", *brokers.begin());
        NodeManager::SetStringAttribute(attrs, "topic_name", *topic_names.begin());
    }
}
