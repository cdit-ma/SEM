#include "environment.h"
#include "node.h"
#include "component.h"
#include "logger.h"
#include "ports/port.h"
#include "attribute.h"
#include "worker.h"
#include "container.h"
#include "helper.h"

using namespace EnvironmentManager;

Node::Node(Environment& environment, Experiment& parent, const NodeManager::Node& node) : 
            environment_(environment),
            experiment_(parent)
{
    name_ = node.info().name();
    id_ = node.info().id();
    ip_ = node.ip_address();

    if(ip_ == "OFFLINE" && node.containers_size() > 0){
        throw std::invalid_argument("Experiment: '" + experiment_.GetName() + "' Has OFFLINE node '" + name_ + "' utilized.");
    }

    for(const auto& container : node.containers()){
        AddContainer(container);
    }
}

std::string Node::GetId() const{
    return id_;
}

std::string Node::GetName() const{
    return name_;
}

std::string Node::GetIp() const{
    return ip_;
}

int Node::GetContainerCount() const{
    return containers_.size();
}

void Node::AddContainer(const NodeManager::Container& container_pb){
    const auto& id = container_pb.info().id();
    if(container_pb.implicit()){
        implicit_container_id_ = id;
    }
    auto container = std::unique_ptr<EnvironmentManager::Container>(new EnvironmentManager::Container(environment_, *this, container_pb));
    containers_.emplace(id, std::move(container));
}

void Node::SetDirty(){
    dirty_ = true;
    GetExperiment().SetDirty();
}

bool Node::IsDirty(){
    return dirty_;
}

bool Node::HasContainer(const std::string& container_id) const{
    return containers_.count(container_id) > 0;
}

Container& Node::GetContainer(const std::string& container_id) const{
    if(containers_.count(container_id)){
        return *(containers_.at(container_id));
    }
    throw std::out_of_range("Node::GetContainer: " + id_ + " GET: "+ container_id);
}

bool Node::HasPort(const std::string& port_id) const{
    for(const auto& container : containers_){
        if(container.second->HasPort(port_id)){
            return true;
        }
    }
    return false;
}

Port& Node::GetPort(const std::string& port_id) const{
    for(const auto& container : containers_){
        if(container.second->HasPort(port_id)){
            return container.second->GetPort(port_id);
        }
    }
    throw std::out_of_range("Node::GetPort: " + id_ + " GET: " + port_id);
}

std::unique_ptr<NodeManager::Node> Node::GetProto(const bool full_update){
    std::unique_ptr<NodeManager::Node> node;

    //Only produce a node if we have Deployed Components?
    if(!containers_.empty() && (dirty_ || full_update)){
        node = std::unique_ptr<NodeManager::Node>(new NodeManager::Node());
        
        node->mutable_info()->set_name(name_);
        node->mutable_info()->set_id(id_);

        node->set_ip_address(ip_);

        for(const auto& container : containers_){
            if(container.second->GetDeployedComponentCount()){
                auto container_pb = container.second->GetProto(full_update);
                if(container_pb){
                    node->mutable_containers()->AddAllocated(container_pb.release());
                }
            }
        }

        if(dirty_){
            dirty_ = false;
        }
    }
    return node;
}

std::unique_ptr<NodeManager::HardwareId> Node::GetHardwareId() const{
    auto hardware_id = std::unique_ptr<NodeManager::HardwareId>(new NodeManager::HardwareId());
    hardware_id->set_host_name(GetName());
    hardware_id->set_ip_address(GetIp());
    return hardware_id;
}

std::vector<std::unique_ptr<NodeManager::ContainerId> > Node::GetComponentContainerIds() const {
    std::vector<std::unique_ptr<NodeManager::ContainerId> > container_ids;
    for(const auto& container : containers_) {
        if(container.second->GetDeployedComponentCount() > 0){
            auto container_id = std::unique_ptr<NodeManager::ContainerId>(new NodeManager::ContainerId());
            container_id->set_id(container.second->GetId());
            container_id->set_is_docker(container.second->IsDocker());
            container_id->set_is_master(container.second->IsNodeManagerMaster());
            container_ids.emplace_back(std::move(container_id));
        }
    }
    return container_ids;
}

Experiment& Node::GetExperiment(){
    return experiment_;
}

int Node::GetDeployedCount() const{
    int temp = 0;
    for(const auto& container : containers_){
        temp += container.second->GetDeployedCount();
    }
    return temp;
}

int Node::GetDeployedComponentCount() const {
    int temp = 0;
    for(const auto& container : containers_) {
        temp += container.second->GetDeployedComponentCount();
    }
    return temp;
}

bool Node::HasLogger(const std::string& logger_id) const {
    for(const auto& container : containers_){
        if(container.second->HasLogger(logger_id)){
            return true;
        }
    }
    return false;
}


std::vector<std::reference_wrapper<Logger> > Node::GetLoggers(const std::string &logger_id) const {
    std::vector<std::reference_wrapper<Logger> > loggers;

    for(const auto& container : containers_){
        try{
            loggers.emplace_back(container.second->GetLogger(logger_id));
        }catch(const std::exception& ex){

        }
    }
    return loggers;
}

int Node::GetLoganServerCount() const {
    int servers = 0;
    for(const auto& container : containers_) {
        servers += container.second->GetLoganServerCount();
    }
    return servers;
}

std::vector<std::unique_ptr<NodeManager::Logger> > Node::GetAllocatedLoganServers() {
    std::vector<std::unique_ptr<NodeManager::Logger> > logan_servers;

    for(const auto& container_pair : containers_) {
        if(container_pair.second->GetLoganServerCount() > 0){
            for(const auto& server_id : container_pair.second->GetLoganServerIds()){

                logan_servers.emplace_back(container_pair.second->GetLogger(server_id).GetProto(true));
            }
        }
    }
    return logan_servers;
}

void Node::AddComponentToImplicitContainer(const NodeManager::Component &component) {
    containers_.at(implicit_container_id_)->AddComponent(component);
}

void Node::AddLoggingClient(const NodeManager::Logger &logging_client) {
    auto& implicit_container = GetContainer(implicit_container_id_);
    if(implicit_container.GetDeployedComponentCount()){
        implicit_container.AddLogger(logging_client);
    }else{
        for(const auto& container_pair : containers_){
            if(container_pair.second->GetDeployedComponentCount()){
                container_pair.second->AddLogger(logging_client);
                break;
            }
        }
    }
}

void Node::AddLoggingServerToImplicitContainer(const NodeManager::Logger &logging_server) {
    containers_.at(implicit_container_id_)->AddLogger(logging_server);
}

bool Node::HasMasterEligibleContainer() const{
    for(const auto& container : containers_){
        if(container.second->GetDeployedComponentCount() && !container.second->IsLateJoiner()){
            return true;
        }
    }
    return false;
}

Container& Node::GetMasterEligibleContainer() const{
    for(const auto& container : containers_){
        if(container.second->GetDeployedComponentCount() && !container.second->IsLateJoiner()){
            return *(container.second);
        }
    }
    throw std::invalid_argument("No eligible container found");
}

std::string Node::GetLogMessage() const {
    return std::string("Node: " + GetName() + " Deploys: " + std::to_string(GetDeployedComponentCount()) + " Components");
}

bool Node::HasNodeManagerMaster() const {
    for(const auto& container_pair : containers_){
        if(container_pair.second->IsNodeManagerMaster()){
            return true;
        }
    }
    return false;
}