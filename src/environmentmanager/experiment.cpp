#include "environment.h"
#include "experiment.h"
#include "node.h"

using namespace EnvironmentManager;

Experiment::Experiment(Environment& environment, const std::string name) : environment_(environment){
    model_name_ = name;
}

Experiment::~Experiment(){
    try{
        for(const auto& external_port_pair : external_port_map_){
            const auto& external_port_label = external_port_pair.second->external_label;
            environment_.RemoveDependentExternalExperiment(model_name_, external_port_label);
        }

        //Delete all nodes, frees all ports in destructors
        node_map_.clear();

        environment_.FreeManagerPort(manager_port_);
        environment_.FreePort(master_ip_address_, master_publisher_port_);
        environment_.FreePort(master_ip_address_, master_registration_port_);
    }
    catch(...){
        std::cerr << "Could not delete deployment :" << model_name_ << std::endl;
    }
    std::cout << "Removed experiment: " << model_name_ << std::endl;
}

void Experiment::SetConfigureDone(){
    std::unique_lock<std::mutex> lock(mutex_);
    configure_done_ = true;
}

bool Experiment::ConfigureDone(){
    std::unique_lock<std::mutex> lock(mutex_);
    return configure_done_;
}

std::string Experiment::GetManagerPort() const{
    return manager_port_;
}

void Experiment::SetManagerPort(const std::string& manager_port){
    manager_port_ = manager_port;
}

void Experiment::SetMasterIp(const std::string& ip){
    master_ip_address_ = ip;
}

void Experiment::AddExternalPorts(const NodeManager::ControlMessage& message){
    for(const auto& external_port : message.external_ports()){
        auto temp = new ExternalPort();
        temp->id = external_port.info().id();
        temp->external_label = external_port.info().name();
        for(const auto& connected_port_id : external_port.connected_ports()){
            temp->connected_ports.push_back(connected_port_id);
        }
        temp->is_blackbox = external_port.is_blackbox();
        external_port_map_[external_port.info().id()] = std::unique_ptr<ExternalPort>(temp);

        external_id_to_internal_id_map_[temp->external_label] = temp->id;

        //TODO: Handle this correctly!
        if(temp->is_blackbox){
            environment_.AddPublicEventPort(model_name_, temp->external_label, "");
        }
    }
}

void Experiment::AddNode(const NodeManager::Node& node){
    auto internal_node = std::unique_ptr<EnvironmentManager::Node>(new EnvironmentManager::Node(environment_, *this, node));


    std::string ip_address = internal_node->GetIp();
    node_map_.emplace(ip_address, std::move(internal_node));
    auto& node_ref = node_map_.at(ip_address);
    node_address_map_.insert({node_ref->GetName(), node_ref->GetIp()});

    //Build logan connection map

    auto deploy_count = node_ref->GetDeployedComponentCount();
    if(deploy_count > 0){
        std::cout << "Experiment[" << model_name_ << "] Node: " << node_ref->GetName() << " Deployed: " << deploy_count << std::endl;
    }
}

void Experiment::ConfigureNodes(){
    //node_map_.at(ip_address)->SetLoganClientAddresses(logan_client_address_map_);

    //node_map_.at(ip_address)->PopulateConnections(connection_map_);
}

bool Experiment::HasDeploymentOn(const std::string& ip_address) const {
    if(node_map_.count(ip_address)){
        return node_map_.at(ip_address)->GetDeployedComponentCount() > 0;
    }
    else{
        throw std::invalid_argument("No node found with ip " + ip_address);
    }
}

NodeManager::EnvironmentMessage* Experiment::GetLoganDeploymentMessage(const std::string& ip_address){
    if(node_map_.count(ip_address)){
        return node_map_.at(ip_address)->GetLoganDeploymentMessage();
    }
}

std::string Experiment::GetMasterPublisherAddress(){
    if(master_publisher_port_.empty()){
        master_publisher_port_ = environment_.GetPort(master_ip_address_);
    }
    return "tcp://" + master_ip_address_ + ":" + master_publisher_port_;
}

std::string Experiment::GetMasterRegistrationAddress(){
    if(master_registration_port_.empty()){
        master_registration_port_ = environment_.GetPort(master_ip_address_);
    }
    return "tcp://" + master_ip_address_ + ":" + master_registration_port_;
}

std::string Experiment::GetNodeModelLoggerPort(const std::string& ip_address) const{
    return node_map_.at(ip_address)->GetModelLoggerPort();
}

std::set<std::string> Experiment::GetTopics() const{
    return topic_set_;
}

std::vector<std::string> Experiment::GetPublisherAddress(const NodeManager::Port& port){

    std::unique_lock<std::mutex> lock(mutex_);
    std::vector<std::string> publisher_addresses;

    if(port.kind() == NodeManager::Port::SUBSCRIBER || port.kind() == NodeManager::Port::REQUESTER){

        if(connection_map_.count(port.info().id())){
            //Get list of connected ports
            auto publisher_port_ids = connection_map_.at(port.info().id());

            //Get those ports addresses
            for(auto id : publisher_port_ids){
                auto node_ip = port_map_.at(id).node_ip;
                auto port_assigned_port = port_map_.at(id).port_number;
                publisher_addresses.push_back("tcp://" + node_ip + ":" + port_assigned_port);
            }
        }

        for(int i = 0; i < port.connected_external_ports_size(); i++){
            auto external_port_label = external_port_map_.at(port.connected_external_ports(i))->external_label;
            //Check environment for public ports with this id
            if(environment_.HasPublicEventPort(external_port_label)){
                publisher_addresses.push_back(environment_.GetPublicEventPortEndpoint(external_port_label));
            }
            //We don't have this public port's address yet. The experiment it orinates from is most likely not started yet.
            //We need to keep track of the fact that this experiment is waiting for this port to become live so we can notify it of the environment change.
            else{
                environment_.AddPendingPublicEventPort(model_name_, external_port_label);
            }
        }
    }

    else if(port.kind() == NodeManager::Port::PUBLISHER || port.kind() == NodeManager::Port::REPLIER){
        auto node_ip = port_map_.at(port.info().id()).node_ip;

        auto port_assigned_port = port_map_.at(port.info().id()).port_number;
        std::string addr_string = "tcp://" + node_ip + ":" + port_assigned_port;
        publisher_addresses.push_back(addr_string);

        //Update any external port references with the address of this port.
        for(int i = 0; i < port.connected_external_ports_size(); i++){
            auto external_port_label = external_port_map_.at(port.connected_external_ports(i))->external_label;
            environment_.AddPublicEventPort(model_name_, external_port_label, addr_string);
        }
    }
    return publisher_addresses;
}

std::string Experiment::GetTaoReplierServerAddress(const NodeManager::Port& port){
    std::unique_lock<std::mutex> lock(mutex_);
    std::string server_address;

    if(port.kind() == NodeManager::Port::REQUESTER){

        //Get list of connected ports
        auto replier_port_ids = connection_map_.at(port.info().id());

        //Get those ports addresses
        for(auto id : replier_port_ids){
            auto& replier_port = port_map_.at(id);
            auto node_ip = replier_port.node_ip;
            auto port_assigned_port = replier_port.port_number;
            auto port_server_name = replier_port.topic;
            server_address = "corbaloc:iiop:" + node_ip + ":" + port_assigned_port + "/" +  port_server_name;
        }
    }

    return server_address;
}

std::string Experiment::GetTaoServerName(const NodeManager::Port& port){
    std::unique_lock<std::mutex> lock(mutex_);
    if(port.kind() == NodeManager::Port::REQUESTER || port.kind() == NodeManager::Port::REPLIER){
        //Get those ports addresses
        auto& replier_port = port_map_.at(port.info().id());
        return replier_port.topic;
    }
    return std::string();
}


std::string Experiment::GetOrbEndpoint(const std::string& port_id){
    // auto node_ip = port_map_.at(port_id).node_ip;

    // return node_ip + ":" + orb_port_map_.at(node_ip);
    return "";
}

bool Experiment::IsDirty() const{
    return dirty_flag_;
}

void Experiment::UpdatePort(const std::string& port_guid){
    dirty_flag_ = true;
    updated_port_ids_.insert(port_guid);
}

void Experiment::SetDeploymentMessage(const NodeManager::ControlMessage& control_message){
    deployment_message_ = NodeManager::ControlMessage(control_message);
}

void Experiment::GetUpdate(NodeManager::ControlMessage& control_message){
    std::unique_lock<std::mutex> lock(mutex_);
    dirty_flag_ = false;

    // while(!updated_port_ids_.empty()){
    //     auto port_it = updated_port_ids_.begin();
    //     auto port_external_id = *port_it;
    //     std::string endpoint = environment_.GetPublicEventPortEndpoint(port_external_id);
    //     updated_port_ids_.erase(port_it);

    //     std::cout << "adding " << port_external_id << std::endl;
    //     std::cout << "adding " << endpoint << std::endl;
    //     auto port_internal_id = external_id_to_internal_id_map_.at(port_external_id);
    //     std::cout << "adding " << port_internal_id << std::endl;

    //     //Iterate through all ports and check for connections to updated ports.
    //     //When we find one, add the new port endpoint to it's list of connecitons.
    //     //XXX:this is pretty terrible...
    //     for(int i = 0; i < deployment_message_.nodes_size(); i++){
    //         std::cout << i << std::endl;
    //         auto node = deployment_message_.nodes(i);
    //         for(int j = 0; j < node.components_size(); j++){
    //         std::cout << j << std::endl;
    //             auto component = node.components(j);
    //             for(int k = 0; k < component.ports_size(); k++){
    //                 auto port = component.mutable_ports(k);
    //         std::cout << k << std::endl;

    //                 for(int l = 0; l < port->connected_external_ports_size(); l++){
    //                     auto connected_port = port->connected_external_ports(l);
    //                     std::cout << connected_port << std::endl;
    //                     if(port_internal_id == connected_port){
    //                         std::cout << "yes" << std::endl;
    //                         for(int m = 0; m < port->attributes_size(); m++){

    //                             auto attribute = port->mutable_attributes(m);
    //                             if(attribute->info().name() == "publisher_address"){
    //                                 std::cout << "adding" << endpoint << " to " << port->info().name()<< std::endl;
    //                                 attribute->add_s(endpoint);
    //                             }
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
    
    control_message.CopyFrom(deployment_message_);
    std::cout << control_message.DebugString() << std::endl;
}

std::string Experiment::GetNodeIpByName(const std::string& node_name){
    try{
        return node_address_map_.at(node_name);
    }
    catch(const std::exception& ex){
        throw std::runtime_error(model_name_ + " has no registered node: '" + node_name + "'");
    }
}

void Experiment::AddLoganClientEndpoint(const std::string& client_id, const std::string& endoint){

}

void Experiment::RemoveLoganClientEndpoint(const std::string& client_id){

}

void Experiment::AddZmqEndpoint(const std::string& port_id, const std::string& endpoint){

}

void Experiment::RemoveZmqEndpoint(const std::string& port_id){

}

void Experiment::AddConnection(const std::string& connected_id, const std::string& port_id){

}

void Experiment::AddTopic(const std::string& topic){
    
}