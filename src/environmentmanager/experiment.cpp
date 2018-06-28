#include "experiment.h"
#include "environment.h"
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
    for(auto& node_pair : node_map_){
        node_pair.second->ConfigureConnections();
    }
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
    /*

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
    */
    return publisher_addresses;
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

    
    control_message.CopyFrom(deployment_message_);
    std::cout << control_message.DebugString() << std::endl;
}

NodeManager::ControlMessage* Experiment::GetProto(){
    NodeManager::ControlMessage* control_message = new NodeManager::ControlMessage();

    control_message->set_experiment_id(model_name_);

    for(auto& node_pair : node_map_){
        if(node_pair.second->DeployedTo()){
            control_message->mutable_nodes()->AddAllocated(node_pair.second->GetProto());
        }
    }
    auto master_ip_address_attribute = control_message->add_attributes();
    auto master_ip_address_attribute_info = master_ip_address_attribute->mutable_info();
    master_ip_address_attribute_info->set_name("master_ip_address");
    master_ip_address_attribute->set_kind(NodeManager::Attribute::STRING);
    master_ip_address_attribute->add_s(master_ip_address_);

    auto master_publisher_port_attribute = control_message->add_attributes();
    auto master_publisher_port_attribute_info = master_publisher_port_attribute->mutable_info();
    master_publisher_port_attribute_info->set_name("master_publisher_endpoint");
    master_publisher_port_attribute->set_kind(NodeManager::Attribute::STRING);
    master_publisher_port_attribute->add_s(GetMasterPublisherAddress());

    auto master_registration_port_attribute = control_message->add_attributes();
    auto master_registration_port_attribute_info = master_registration_port_attribute->mutable_info();
    master_registration_port_attribute_info->set_name("master_registration_endpoint");
    master_registration_port_attribute->set_kind(NodeManager::Attribute::STRING);
    master_registration_port_attribute->add_s(GetMasterRegistrationAddress());

    return control_message;
}

void Experiment::AddModelLoggerEndpoint(const std::string& node_id, const std::string& endpoint){
    modellogger_endpoint_map_.insert(std::make_pair(node_id, endpoint));
}

void Experiment::RemoveModelLoggerEndpoint(const std::string& node_id){

}

const std::unordered_map<std::string, std::string>& Experiment::GetModelLoggerEndpointMap() const{
    return modellogger_endpoint_map_;
}

void Experiment::AddLoganClientEndpoint(const std::string& client_id, const std::string& endpoint){
    logan_client_endpoint_map_.insert(std::make_pair(client_id, endpoint));
}

void Experiment::RemoveLoganClientEndpoint(const std::string& client_id){

}

const std::unordered_map<std::string, std::string>& Experiment::GetLoganClientEndpointMap() const{
    return logan_client_endpoint_map_;
}

void Experiment::AddZmqEndpoint(const std::string& port_id, const std::string& endpoint){
    zmq_endpoint_map_.insert(std::make_pair(port_id, endpoint));
}

void Experiment::RemoveZmqEndpoint(const std::string& port_id){

}

const std::unordered_map<std::string, std::string>& Experiment::GetZmqEndpointMap() const{
    return zmq_endpoint_map_;
}

void Experiment::AddTaoEndpoint(const std::string& port_id, const std::string& endpoint){
    tao_endpoint_map_.insert(std::make_pair(port_id, endpoint));
}

void Experiment::RemoveTaoEndpoint(const std::string& port_id){

}

const std::unordered_map<std::string, std::string>& Experiment::GetTaoEndpointMap() const{
    return tao_endpoint_map_;
}

void Experiment::AddConnection(const std::string& connected_id, const std::string& port_id){

}

void Experiment::AddTopic(const std::string& topic){

}