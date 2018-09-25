#include "experiment.h"
#include "environment.h"
#include "node.h"
#include "ports/port.h"
#include <proto/controlmessage/helper.h>

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

Environment& Experiment::GetEnvironment() const{
    return environment_;
}

void Experiment::SetConfigured(){
    std::unique_lock<std::mutex> lock(mutex_);
    is_configured_ = true;
}

bool Experiment::IsConfigured(){
    std::unique_lock<std::mutex> lock(mutex_);
    return is_configured_;
}

bool Experiment::IsRunning(){
    std::unique_lock<std::mutex> lock(mutex_);
    return is_running_;
}

void Experiment::SetRunning(){
    std::unique_lock<std::mutex> lock(mutex_);
    is_running_ = true;
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
        if(!external_port.is_blackbox()){
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
        else{
            const auto& internal_id = external_port.info().id();
            auto port = Port::ConstructBlackboxPort(*this, external_port);
            if(port){
                blackbox_port_map_.emplace(internal_id, std::move(port));
            }
        }
    }
}

Node& Experiment::GetNodeManagerMaster() const{

}

Node& Experiment::GetNode(const std::string& ip_address) const{
    try{
        return *(node_map_.at(ip_address));
    }catch(const std::exception& ex){
        throw std::invalid_argument("Experiment: '" + model_name_ + "' does not have registered node with IP: '" + ip_address + "'");
    }
}

void Experiment::AddNode(const NodeManager::Node& node){
    if(node.type() == NodeManager::Node::HARDWARE_NODE){
        const auto& ip_address = NodeManager::GetAttribute(node.attributes(), "ip_address").s(0);

        if(!node_map_.count(ip_address)){
            auto internal_node = std::unique_ptr<EnvironmentManager::Node>(new EnvironmentManager::Node(environment_, *this, node));
            const auto& ip_address = internal_node->GetIp();
            node_map_.emplace(ip_address, std::move(internal_node));
            auto& node_ref = node_map_.at(ip_address);
            
            
            //Build logan connection map
            auto deploy_count = node_ref->GetDeployedCount();

            if(deploy_count > 0){
                std::cout << "* Experiment[" << model_name_ << "] Node: " << node_ref->GetName();
                if(GetMasterIp().empty()){
                    node_ref->SetNodeManagerMaster();
                    SetMasterIp(ip_address);
                    std::cout << " [RE_MASTER]";
                }
                std::cout << " Deploys: " << deploy_count << " Components" << std::endl;
            }
        }
        else{
            throw std::invalid_argument("Experiment: '" + model_name_ + "' Got duplicate node with ip address: '" + ip_address + "'");
        }
    }
}

bool Experiment::HasDeploymentOn(const std::string& ip_address) const {
    return GetNode(ip_address).GetDeployedCount() > 0;
}

std::unique_ptr<NodeManager::RegisterExperimentReply> Experiment::GetDeploymentInfo(){
    auto reply = std::unique_ptr<NodeManager::RegisterExperimentReply>(new NodeManager::RegisterExperimentReply());

    for(const auto& node_pair : node_map_){
        auto& node = (*node_pair.second);

        if(node.GetLoganServerCount()){
            auto hardware_id = node.GetHardwareId();
            if(hardware_id){
                reply->add_logan_servers()->Swap(hardware_id.get());
            }
        }

        if(node.GetDeployedComponentCount()){
            auto hardware_id = node.GetHardwareId();
            if(hardware_id){
                reply->add_node_managers()->Swap(hardware_id.get());
            }
        }
    }
    return reply;
}



std::string Experiment::GetMasterPublisherAddress(){
    if(master_publisher_port_.empty()){
        master_publisher_port_ = environment_.GetPort(master_ip_address_);
    }
    return "tcp://" + master_ip_address_ + ":" + master_publisher_port_;
}

const std::string& Experiment::GetMasterIp() const{
    return master_ip_address_;
}

std::string Experiment::GetMasterRegistrationAddress(){
    if(master_registration_port_.empty()){
        master_registration_port_ = environment_.GetPort(master_ip_address_);
    }
    return "tcp://" + master_ip_address_ + ":" + master_registration_port_;
}

bool Experiment::IsDirty() const{
    return dirty_;
}

void Experiment::SetDirty(){
    dirty_ = true;
}

void Experiment::Shutdown(){
    if(!shutdown_){
        shutdown_ = true;
        SetDirty();
    }
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
    for(const auto& port_pair : blackbox_port_map_){
        if(port_pair.first == id){
            return *port_pair.second;
        }
    }
    throw std::out_of_range("Experiment::GetPort: <" + id + "> OUT OF RANGE");
}

std::unique_ptr<NodeManager::EnvironmentMessage> Experiment::GetProto(const bool full_update){
    std::unique_ptr<NodeManager::EnvironmentMessage> environment_message;

    if(dirty_ || full_update){
        environment_message = std::unique_ptr<NodeManager::EnvironmentMessage>(new NodeManager::EnvironmentMessage());

        if(!shutdown_){
            environment_message->set_type(NodeManager::EnvironmentMessage::CONFIGURE_EXPERIMENT);

            auto control_message = environment_message->mutable_control_message();
            control_message->set_experiment_id(model_name_);
            for(auto& node_pair : node_map_){
                if(node_pair.second->DeployedTo()){
                    auto node_pb = node_pair.second->GetProto(full_update);
                    if(node_pb){
                        control_message->mutable_nodes()->AddAllocated(node_pb.release());
                    }
                }
            }

            auto attrs = control_message->mutable_attributes();
            NodeManager::SetStringAttribute(attrs, "master_ip_address", GetMasterIp());
            NodeManager::SetStringAttribute(attrs, "master_publisher_endpoint", GetMasterPublisherAddress());
            NodeManager::SetStringAttribute(attrs, "master_registration_endpoint", GetMasterRegistrationAddress());
        }else{
            //Terminate the experiment
            environment_message->set_type(NodeManager::EnvironmentMessage::SHUTDOWN_EXPERIMENT);
        }

        if(dirty_){
            dirty_ = false;
        }
    }
    return environment_message;
}

std::vector<std::unique_ptr<NodeManager::Logger> > Experiment::GetAllocatedLoganServers(const std::string& ip_address){
    return std::move(GetNode(ip_address).GetAllocatedLoganServers());
}

EnvironmentManager::ExternalPort& Experiment::GetExternalPort(const std::string& external_port_internal_id){
    if(external_port_map_.count(external_port_internal_id)){
        return *external_port_map_.at(external_port_internal_id);
    }
    else{
        throw std::invalid_argument("Experiment: '" + model_name_ + "' doesn't have external port id: '" + external_port_internal_id + "'");
    }
}

void Experiment::AddExternalConsumerPort(const std::string& external_port_internal_id, const std::string& internal_port_id){
    try{
        auto& external_port = GetExternalPort(external_port_internal_id);
        environment_.AddExternalConsumerPort(model_name_, external_port.external_label);
        external_port.consumer_ids.insert(internal_port_id);
    }
    catch(const std::exception& ex){
    }
}

void Experiment::AddExternalProducerPort(const std::string& external_port_internal_id, const std::string& internal_port_id){
    try{
        auto& external_port = GetExternalPort(external_port_internal_id);
        environment_.AddExternalProducerPort(model_name_, external_port.external_label);
        external_port.producer_ids.insert(internal_port_id);
    }
    catch(const std::exception& ex){
    }
}

void Experiment::RemoveExternalConsumerPort(const std::string& external_port_internal_id, const std::string& internal_port_id){
    try{
        auto& external_port = GetExternalPort(external_port_internal_id);
        environment_.RemoveExternalConsumerPort(model_name_, external_port.external_label);
        external_port.consumer_ids.erase(internal_port_id);
    }
    catch(const std::exception& ex){
    }
}

void Experiment::RemoveExternalProducerPort(const std::string& external_port_internal_id, const std::string& internal_port_id){
    try{
        auto& external_port = GetExternalPort(external_port_internal_id);
        environment_.RemoveExternalProducerPort(model_name_, external_port.external_label);
        external_port.producer_ids.erase(internal_port_id);
    }
    catch(const std::exception& ex){
    }
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