#include "taoport.h"

using namespace EnvironmentManager::tao;


Port(Component& parent, const NodeManager::Port& port):
    ::EnvironmentManager::Port(parent, port){

    const auto& orb_port = GetNode().AssignOrbPort();
    
    

        std::string topic = GetName() + "_" + GetId();
        tao_server_name_list_.push_back(topic);

        //TODO: Check if we are using a topic_name
        SetTopic(topic);
    }

}

Port(Experiment& parent, const NodeManager::ExternalPort& port):
    ::EnvironmentManager::Port(parent, port){

    for(const auto& server_name_segment : port.server_name()){
        server_name_.emplace_back(server_name_segment);
    }
}

std::vector<std::string> Port::GetServerName() const{
    return server_name_;
}

std::string Port::GetNamingServiceEndpoint() const{
    return naming_service_endpoint_;
}

std::string Port::GetOrbEndpoint() const{
    return orb_endpoint_;
}

void Port::FillPortPb(NodeManager::Port& port_pb){
    auto& node = GetNode();

    if(true /*TODO: IF !BLACKBOX*/){
        auto orb_pb = port_pb.add_attributes();
        orb_pb->mutable_info()->set_name("orb_endpoint");
        orb_pb->set_kind(NodeManager::Attribute::STRING);
        orb_pb->add_s(GetOrbEndpoint());
    }

    //If we're a replier, we can set our server name(topic) and naming service endpoint based on internal information.
    if(GetKind() == Kind::Replier){
        //Set the Server Name Attribute
        auto server_name_pb = port_pb.add_attributes();
        server_name_pb->mutable_info()->set_name("server_name");
        server_name_pb->set_kind(NodeManager::Attribute::STRINGLIST);

        for(const auto& server_name : GetServerName()){
            server_name_pb->add_s(server_name);
        }

        //Set the Naming Service Endpoint Attribute
        auto naming_service_pb = port_pb.add_attributes();
        naming_service_pb->mutable_info()->set_name("naming_service_endpoint");
        naming_service_pb->set_kind(NodeManager::Attribute::STRING);
        naming_service_pb->add_s(GetNamingServiceEndpoint());
    }

    //If we're a requester, get server name and naming service from connected replier port.
    if(port.GetKind() == Kind::Requester){
        //Connect all Internal ports
        for(auto connected_port : GetConnectedPorts()){
            //Set the Server Name Attribute
            auto server_name_pb = port_pb.add_attributes();
            server_name_pb->mutable_info()->set_name("server_name");
            server_name_pb->set_kind(NodeManager::Attribute::STRINGLIST);

            for(const auto& server_name : connected_port.GetServerName()){
                server_name_pb->add_s(server_name);
            }

            //Set the Naming Service Endpoint Attribute
            auto naming_service_pb = port_pb.add_attributes();
            naming_service_pb->mutable_info()->set_name("naming_service_endpoint");
            naming_service_pb->set_kind(NodeManager::Attribute::STRING);
            naming_service_pb->add_s(connected_port.GetNamingServiceEndpoint());
            break;
        }
    }
}