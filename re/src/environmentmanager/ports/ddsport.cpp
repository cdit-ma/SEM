#include "ddsport.h"
#include "../environment.h"
#include "../experiment.h"
#include "helper.h"



using namespace EnvironmentManager::dds;

Port::Port(::EnvironmentManager::Component& parent, const NodeManager::Port& port) :
    ::EnvironmentManager::Port(parent, port){
    topic_name_ = NodeManager::GetAttribute(port.attributes(), "topic_name").s(0);
}

Port::Port(::EnvironmentManager::Experiment& parent, const NodeManager::ExternalPort& port) :
    ::EnvironmentManager::Port(parent, port){
    topic_name_ = NodeManager::GetAttribute(port.attributes(), "topic_name").s(0);
    domain_id_ = NodeManager::GetAttribute(port.attributes(), "domain_id").i();
}

std::string Port::GetTopic() const{
    return topic_name_;
}

int Port::GetDomainId() const{
    return domain_id_;
}

void Port::FillPortPb(NodeManager::Port& port_pb){
    auto attrs = port_pb.mutable_attributes();
    
    const auto& kind = GetKind();

    if(kind == Kind::Publisher){
        std::set<int> blackbox_domain_ids;
        std::set<std::string> blackbox_topic_names;

        int domain_id = GetDomainId();
        std::string topic_name = GetTopic();

        //Populate publish destination info based on any connection to blackbox ports.
        for(const auto& connected_port_ref : GetConnectedPorts()){
            const auto& connected_port = connected_port_ref.get();
            const auto& dds_port = (const Port&) connected_port;

            if(connected_port.IsBlackbox()){
                blackbox_domain_ids.insert(dds_port.GetDomainId());
                blackbox_topic_names.insert(dds_port.GetTopic());
            }
        }

        if(blackbox_domain_ids.size() > 0){
            if(blackbox_domain_ids.size() > 1){
                std::cerr << "* Experiment[" << GetExperiment().GetName() << "]: Has multiple blackbox domain ids connected to Port: '" << GetId() << "'" << std::endl;
            }
            std::cerr << "* Experiment[" << GetExperiment().GetName() << "]: Has blackbox connected to port: '" << GetId() << "'. Overriding broker address!" << std::endl;
            domain_id = *(blackbox_domain_ids.begin());
        }

        if(blackbox_topic_names.size() > 0){
            if(blackbox_topic_names.size() > 1){
                std::cerr << "* Experiment[" << GetExperiment().GetName() << "]: Has multiple blackbox topics connected to Port: '" << GetId() << "'" << std::endl;
            }
            std::cerr << "* Experiment[" << GetExperiment().GetName() << "]: Has blackbox connected to port: '" << GetId() << "'. Overriding topic name!" << std::endl;
            topic_name = *(blackbox_topic_names.begin());
        }

        NodeManager::SetStringAttribute(attrs, "topic_name", topic_name);
        NodeManager::SetIntegerAttribute(attrs, "domain_id", domain_id);
    }
    else if(kind == Kind::Subscriber){
        std::set<std::string> topic_names;
        std::set<int> domain_ids;

        //Connect all Internal ports
        for(const auto& connected_port_ref : GetConnectedPorts()){
            const auto& connected_port = connected_port_ref.get();
            const auto& dds_port = (const Port&) connected_port;

            domain_ids.insert(dds_port.GetDomainId());
            topic_names.insert(dds_port.GetTopic());
        }

        //If we have any size other than ONE for either list, warn user.
        if(topic_names.size() > 1){
            std::cerr << "* Experiment[" << GetExperiment().GetName() << "]: Has multiple topics connected to Port: '" << GetId() << "'" << std::endl;
        }
        if(topic_names.size() == 0){
            std::cerr << "* Experiment[" << GetExperiment().GetName() << "]: Port: '" << GetId() << "' Has no connected ports, defaulting to user set topic_name." << std::endl;
        }

        if(domain_ids.size() > 1){
            std::cerr << "* Experiment[" << GetExperiment().GetName() << "]: Has multiple domains connected to Port: '" << GetId() << "'" << std::endl;
        }
        if(domain_ids.size() == 0){
            std::cerr << "* Experiment[" << GetExperiment().GetName() << "]: Port: '" << GetId() << "' Has no connected ports, defaulting to user set domain_id." << std::endl;
        }

        //Set topic name and domain id. If none found based on connected ports, use value set on port.
        int domain_id = domain_ids.size() > 0 ? *domain_ids.begin() : GetDomainId();
        NodeManager::SetIntegerAttribute(attrs, "domain_id", domain_id);

        std::string topic_name = topic_names.size() > 0 ? *topic_names.begin() : GetTopic();
        NodeManager::SetStringAttribute(attrs, "topic_name", topic_name);
    }
}
