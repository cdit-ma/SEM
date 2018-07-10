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
}

const std::string& Experiment::GetName() const{
    return model_name_;
}

void Experiment::SetConfigured(){
    std::unique_lock<std::mutex> lock(mutex_);
    is_configured_ = true;
}

bool Experiment::IsConfigured(){
    std::unique_lock<std::mutex> lock(mutex_);
    return is_configured_;
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
            external_port_map_.emplace(internal_id, std::unique_ptr<ExternalPort>(port));
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
                
            //Build logan connection map
            auto deploy_count = node_ref->GetDeployedComponentCount();
            if(deploy_count > 0){
                std::cout << "* Experiment[" << model_name_ << "] Node: " << node_ref->GetName() << " Deploys: " << deploy_count << " Components" << std::endl;
            }
        }
        else{
            throw std::invalid_argument("Experiment: '" + model_name_ + "' Got duplicate node with ip address: '" + ip_address + "'");
        }
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
        throw std::invalid_argument("Experiment: '" + model_name_ + "' Doesn't have a Node with IP address: '" + ip_address + "'");
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

bool Experiment::IsDirty() const{
    return dirty_flag_;
}

void Experiment::SetDirty(){
    dirty_flag_ = true;
}

void Experiment::UpdatePort(const std::string& external_port_label){
    if(IsConfigured()){
        if(external_id_to_internal_id_map_.count(external_port_label)){
            const auto& internal_id = external_id_to_internal_id_map_.at(external_port_label);

            const auto& external_port = GetExternalPort(internal_id);

            for(const auto& port_id : external_port.consumer_ids){
                GetPort(port_id).SetDirty();
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

NodeManager::ControlMessage* Experiment::GetUpdate(){
    std::unique_lock<std::mutex> lock(mutex_);
    if(dirty_flag_){
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
        dirty_flag_ = false;
        return control_message;
    }
    return nullptr;
}

NodeManager::ControlMessage* Experiment::GetProto(){
    NodeManager::ControlMessage* control_message = new NodeManager::ControlMessage();

    control_message->set_experiment_id(model_name_);

    for(auto& node_pair : node_map_){
        if(node_pair.second->DeployedTo()){
            control_message->mutable_nodes()->AddAllocated(node_pair.second->GetProto());
        }
    }

    auto master_ip_pb = control_message->add_attributes();
    master_ip_pb->mutable_info()->set_name("master_ip_address");
    master_ip_pb->set_kind(NodeManager::Attribute::STRING);
    master_ip_pb->add_s(master_ip_address_);

    auto master_pub_pb = control_message->add_attributes();
    master_pub_pb->mutable_info()->set_name("master_publisher_endpoint");
    master_pub_pb->set_kind(NodeManager::Attribute::STRING);
    master_pub_pb->add_s(GetMasterPublisherAddress());

    auto master_reg_pb = control_message->add_attributes();
    master_reg_pb->mutable_info()->set_name("master_registration_endpoint");
    master_reg_pb->set_kind(NodeManager::Attribute::STRING);
    master_reg_pb->add_s(GetMasterRegistrationAddress());

    return control_message;
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
    external_port.consumer_ids.insert(internal_port_id);
}

void Experiment::AddExternalProducerPort(const std::string& external_port_internal_id, const std::string& internal_port_id){
    auto& external_port = GetExternalPort(external_port_internal_id);
    environment_.AddExternalProducerPort(model_name_, external_port.external_label);
    external_port.producer_ids.insert(internal_port_id);
}

void Experiment::RemoveExternalConsumerPort(const std::string& external_port_internal_id, const std::string& internal_port_id){
    auto& external_port = GetExternalPort(external_port_internal_id);
    environment_.RemoveExternalConsumerPort(model_name_, external_port.external_label);
    external_port.consumer_ids.erase(internal_port_id);
}

void Experiment::RemoveExternalProducerPort(const std::string& external_port_internal_id, const std::string& internal_port_id){
    auto& external_port = GetExternalPort(external_port_internal_id);
    environment_.RemoveExternalProducerPort(model_name_, external_port.external_label);
    external_port.producer_ids.erase(internal_port_id);
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

std::vector< std::reference_wrapper<Logger> > Experiment::GetLoggerClients(const std::string& logger_id){
    std::vector< std::reference_wrapper<Logger> > loggers;
    
    for(auto& node_pair : node_map_){
        auto& node = node_pair.second;
        if(node->HasLogger(logger_id)){
            loggers.emplace_back(node->GetLogger(logger_id));
        }
    }

    return loggers;
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