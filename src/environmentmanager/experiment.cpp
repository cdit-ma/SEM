#include "experiment.h"
#include "environment.h"
#include "node.h"
#include "port.h"

using namespace EnvironmentManager;

Experiment::Experiment(Environment& environment, const std::string name) : environment_(environment){
    model_name_ = name;
}

Experiment::~Experiment(){
    try{
        for(const auto& external_port_pair : external_port_map_){
            const auto& port = external_port_pair.second;
            const auto& external_port_label = port->external_label;

            if(port->consumer_ids.size() > 0){
                environment_.RemoveExternalConsumerPort(model_name_, external_port_label);
            }
            if(port->producer_ids.size() > 0){
                environment_.RemoveExternalProducerPort(model_name_, external_port_label);
            }
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
        const auto& internal_id = external_port.info().id();
        if(!external_port_map_.count(internal_id)){
            auto port = new ExternalPort();
            port->internal_id = internal_id;
            port->external_label = external_port.info().name();
            external_port_map_.emplace(internal_id, port);
            external_id_to_internal_id_map_[port->external_label] = internal_id;
        }else{
            throw std::invalid_argument("Experiment: '" + model_name_ + "' Got duplicate external port id: '" + internal_id + "'");
        }
    }
}

void Experiment::AddNode(const NodeManager::Node& node){
    if(node.type() == NodeManager::Node::HARDWARE_NODE){
        auto ip_address = Environment::GetAttributeByName(node.attributes(), "ip_address").s(0);

        if(!node_map_.count(ip_address)){
            auto internal_node = std::unique_ptr<EnvironmentManager::Node>(new EnvironmentManager::Node(environment_, *this, node));
            const auto& ip_address = internal_node->GetIp();
            node_map_.emplace(ip_address, std::move(internal_node));
            auto& node_ref = node_map_.at(ip_address);
            node_address_map_.insert({node_ref->GetName(), node_ref->GetIp()});
                
            //Build logan connection map
            auto deploy_count = node_ref->GetDeployedComponentCount();
            if(deploy_count > 0){
                std::cout << "* Experiment[" << model_name_ << "] Node: " << node_ref->GetName() << " Deployed: " << deploy_count << " Components" << std::endl;
            }
        }
        else{
            throw std::invalid_argument("Experiment: '" + model_name_ + "' Got duplicate node with ip address: '" + ip_address + "'");
        }
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
    else{
        throw std::invalid_argument("Experiment::GetLoganDeploymentMessage No node found with ip " + ip_address);
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

std::set<std::string> Experiment::GetTopics() const{
    return topic_set_;
}

bool Experiment::IsDirty() const{
    return dirty_flag_;
}

void Experiment::SetDirty(){
    dirty_flag_ = true;
}

void Experiment::UpdatePort(const std::string& external_port_label){
    if(configure_done_){
        if(external_id_to_internal_id_map_.count(external_port_label)){
            const auto& internal_id = external_id_to_internal_id_map_.at(external_port_label);

            const auto& external_port = GetExternalPort(internal_id);

            for(const auto& port_id : external_port.consumer_ids){
                GetPort(port_id).UpdateExternalEndpoints();
            }
        }
    }
}

Port& Experiment::GetPort(const std::string& id){
    for(const auto& node_pair : node_map_){
        if(node_pair.second->HasPort(id)){
            return node_pair.second->GetPort(id);
        }
    }
    throw std::out_of_range("Experiment::GetPort: <" + id + "> OUT OF RANGE");
}

std::string Experiment::GetPublicEventPortName(const std::string& public_port_local_id){
    if(external_port_map_.count(public_port_local_id)){
        return external_port_map_.at(public_port_local_id)->external_label;
    }
    return std::string();
}

void Experiment::SetDeploymentMessage(const NodeManager::ControlMessage& control_message){
    deployment_message_ = NodeManager::ControlMessage(control_message);
}

NodeManager::ControlMessage* Experiment::GetUpdate(){
    std::unique_lock<std::mutex> lock(mutex_);
    dirty_flag_ = false;

    auto control_message = new NodeManager::ControlMessage();
    control_message->set_experiment_id(model_name_);

    for(auto& node_pair : node_map_){
        if(node_pair.second->DeployedTo()){
            auto node_update = node_pair.second->GetUpdate();
            if(node_update){
                control_message->mutable_nodes()->AddAllocated(node_update);
            }
        }
    }

    return control_message;
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
    modellogger_endpoint_map_.erase(node_id);
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

EnvironmentManager::ExternalPort& Experiment::GetExternalPort(const std::string& external_port_internal_id){
    if(external_port_map_.count(external_port_internal_id)){
        return *external_port_map_.at(external_port_internal_id);
    }else{
        throw std::invalid_argument("Experiment: '" + model_name_ + "' doesn't have external port id: '" + external_port_internal_id + "'");
    }
}

void Experiment::AddExternalConsumerPort(const std::string& external_port_internal_id, const std::string& internal_port_id){
    auto& external_port = GetExternalPort(external_port_internal_id);
    environment_.AddExternalConsumerPort(model_name_, external_port.external_label);
    std::cerr << model_name_ << " Added External Consumer: " << internal_port_id << " FOR: " << external_port.external_label << std::endl;
    external_port.consumer_ids.insert(internal_port_id);
}

void Experiment::AddExternalProducerPort(const std::string& external_port_internal_id, const std::string& internal_port_id){
    auto& external_port = GetExternalPort(external_port_internal_id);
    environment_.AddExternalProducerPort(model_name_, external_port.external_label);
    std::cerr << model_name_ << " Added External Producer: " << internal_port_id << " FOR: " << external_port.external_label << std::endl;
    external_port.producer_ids.insert(internal_port_id);
}

void Experiment::RemoveExternalConsumerPort(const std::string& external_port_internal_id, const std::string& internal_port_id){
    auto& external_port = GetExternalPort(external_port_internal_id);
    environment_.RemoveExternalConsumerPort(model_name_, external_port.external_label);
    external_port.consumer_ids.erase(internal_port_id);
    std::cerr << model_name_ << " Removed External Consumer: " << internal_port_id << " FOR: " << external_port.external_label << std::endl;
}

void Experiment::RemoveExternalProducerPort(const std::string& external_port_internal_id, const std::string& internal_port_id){
    auto& external_port = GetExternalPort(external_port_internal_id);
    environment_.RemoveExternalProducerPort(model_name_, external_port.external_label);
    external_port.producer_ids.erase(internal_port_id);
    std::cerr << model_name_ << " Removed External Producer: " << internal_port_id << " FOR: " << external_port.external_label << std::endl;
}

std::vector< std::reference_wrapper<Port> > Experiment::GetExternalProducerPorts(const std::string& external_port_label){
    std::vector< std::reference_wrapper<Port> > producer_ports;
    
    try{
        const auto& internal_port_id = GetExternalPortInternalId(external_port_label);
        
        const auto& external_port = GetExternalPort(internal_port_id);
        for(const auto& port_id : external_port.producer_ids){
            producer_ports.emplace_back(GetPort(port_id));
        }
    }catch(const std::runtime_error& ex){
    }
    
    return producer_ports;
}

std::string Experiment::GetExternalPortLabel(const std::string& internal_port_id){
    auto& external_port = GetExternalPort(internal_port_id);
    return external_port.external_label;
}
std::string Experiment::GetExternalPortInternalId(const std::string& external_port_label){
    if(external_id_to_internal_id_map_.count(external_port_label)){
        return external_id_to_internal_id_map_.at(external_port_label);
    }
    throw std::runtime_error("Experiment: '" + model_name_ + "' doesn't have an external port with label '" + external_port_label + "'");
}