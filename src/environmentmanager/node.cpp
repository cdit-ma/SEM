#include "environment.h"
#include "node.h"
#include "component.h"
#include "logger.h"
#include "ports/port.h"
#include "attribute.h"
#include "worker.h"
#include <proto/controlmessage/helper.h>

using namespace EnvironmentManager;

Node::Node(Environment& environment, Experiment& parent, const NodeManager::Node& node) : 
            environment_(environment), experiment_(parent){
    
    name_ = node.info().name();
    id_ = node.info().id();
    ip_ = NodeManager::GetAttribute(node.attributes(), "ip_address").s(0);

    bool utilized = node.loggers_size() + node.components_size() > 0;

    if(ip_ == "OFFLINE" && utilized){
        throw std::invalid_argument("Experiment: '" + experiment_.GetName() + "' Has OFFLINE node '" + name_ + "' utilized.");
    }

    for(const auto& logger : node.loggers()){
        AddLogger(logger);
    }

    for(const auto& component : node.components()){
        AddComponent(component);
    }
    
    for(const auto& attribute : node.attributes()){
        AddAttribute(attribute.second);
    }

    //set logger port
    if(components_.size()){
        AddModelLogger();
    }
    
    //set master/slave port
    if(DeployedTo()){
        management_port_ = environment_.GetPort(ip_);
    }
}

Node::~Node(){
    if(DeployedTo()){
        //Free node's management port
        environment_.FreePort(ip_, GetManagementPort());
        //Free node's orb port
        if(HasOrbPort()){
            environment_.FreePort(ip_, GetOrbPort());
        }
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
int Node::GetDeployedComponentCount() const{
    return components_.size();
}

int Node::GetDeployedCount() const {
    return GetDeployedComponentCount() + GetLoganClientCount();
}

int Node::GetLoganServerCount() const{
    int server_count = 0;
    for(const auto& log_pair : loggers_){
        if(log_pair.second->GetType() == Logger::Type::Server){
            server_count ++;
        }
    }
    return server_count;
}

int Node::GetLoganClientCount() const{
    int client_count = 0;
    for(const auto& log_pair : loggers_){
        if(log_pair.second->GetType() == Logger::Type::Client){
            client_count ++;
        }
    }
    return client_count;
}



bool Node::IsNodeManagerMaster() const{
    return is_node_manager_master_;
}

void Node::SetNodeManagerMaster(){
    is_node_manager_master_ = true;
}

//Port getters/setters
std::string Node::GetManagementPort() const{
    return management_port_;
}
bool Node::HasOrbPort() const{
    return !orb_port_.empty();
}

std::string Node::GetOrbPort() const{
    return orb_port_;
}

std::string Node::AssignOrbPort(){
    if(!HasOrbPort()){
        orb_port_ = environment_.GetPort(ip_);
    }
    return orb_port_;
}

void Node::SetOrbPort(const std::string& orb_port){
    orb_port_ = orb_port;
}

void Node::AddComponent(const NodeManager::Component& component_pb){
    const auto& id = component_pb.info().id();
    auto component = std::unique_ptr<EnvironmentManager::Component>(new EnvironmentManager::Component(environment_, *this, component_pb));
    components_.emplace(id, std::move(component));
}

void Node::AddLogger(const NodeManager::Logger& logger_pb){
    const auto& id = logger_pb.id();
    auto logger = std::unique_ptr<EnvironmentManager::Logger>(new EnvironmentManager::Logger(environment_, *this, logger_pb));
    loggers_.emplace(id, std::move(logger));
}

bool Node::HasLogger(const std::string& logger_id){
    return loggers_.count(logger_id) > 0;
}

Logger& Node::GetLogger(const std::string& logger_id){
    if(HasLogger(logger_id)){
        return *loggers_.at(logger_id);
    }
    throw std::runtime_error("No Logger with ID: '" + logger_id + "'");
}


void Node::AddModelLogger(){
    const auto& id = "model_logger";
    auto logger = std::unique_ptr<EnvironmentManager::Logger>(
        new EnvironmentManager::Logger(environment_,*this, EnvironmentManager::Logger::Type::Model, EnvironmentManager::Logger::Mode::Cached));
    loggers_.emplace(id, std::move(logger));
}

void Node::AddAttribute(const NodeManager::Attribute& attribute_pb){
    const auto& id = attribute_pb.info().id();
    auto attribute = std::unique_ptr<EnvironmentManager::Attribute>(new EnvironmentManager::Attribute(attribute_pb));
    attributes_.emplace(id, std::move(attribute));
}

void Node::SetDirty(){
    dirty_ = true;
    GetExperiment().SetDirty();
}

bool Node::IsDirty(){
    return dirty_;
}

bool Node::HasComponent(const std::string& component_id) const{
    return components_.count(component_id);
}

Component& Node::GetComponent(const std::string& component_id) const{
    if(components_.count(component_id)){
        return *(components_.at(component_id));
    }
    throw std::out_of_range("Node::GetComponent: " + id_ + " GET: "+ component_id);
}

bool Node::HasPort(const std::string& port_id) const{
    for(const auto& component : components_){
        if(component.second->HasPort(port_id)){
            return true;
        }
    }
    return false;
}

Port& Node::GetPort(const std::string& port_id) const{
    for(const auto& component : components_){
        if(component.second->HasPort(port_id)){
            return component.second->GetPort(port_id);
        }
    }
    throw std::out_of_range("Node::GetPort: " + id_ + " GET: " + port_id);
}

std::unique_ptr<NodeManager::Node> Node::GetProto(const bool full_update){
    std::unique_ptr<NodeManager::Node> node;

    //Only produce a node if we have Deployed Components?
    if(GetDeployedComponentCount() && (dirty_ || full_update)){
        node = std::unique_ptr<NodeManager::Node>(new NodeManager::Node());
        
        node->mutable_info()->set_name(name_);
        node->mutable_info()->set_id(id_);
        node->set_type(NodeManager::Node::HARDWARE_NODE);



        for(const auto& logger : loggers_){
            auto logger_pb = logger.second->GetProto(full_update);
            if(logger_pb){
                node->mutable_loggers()->AddAllocated(logger_pb.release());
            }
        }

        for(const auto& component : components_){
            auto component_pb = component.second->GetProto(full_update);
            if(component_pb){
                node->mutable_components()->AddAllocated(component_pb.release());
            }
        }

        //Set the Node Attributes
        auto attrs = node->mutable_attributes();
        NodeManager::SetStringAttribute(attrs, "ip_address", GetIp());
        NodeManager::SetStringAttribute(attrs, "management_port", GetManagementPort());

        for(const auto& attribute : attributes_){
            auto attribute_pb = attribute.second->GetProto(full_update);
            if(attribute_pb){
                NodeManager::AddAllocatedAttribute(attrs, std::move(attribute_pb));
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

std::vector<std::unique_ptr<NodeManager::Logger> > Node::GetAllocatedLoganServers() const{
    std::vector<std::unique_ptr<NodeManager::Logger> > logan_servers;
    
    for(const auto& logger_pair : loggers_){
        auto& logger = logger_pair.second;
        if(logger->GetType() == Logger::Type::Server){
            auto logger_pb = logger->GetProto(true);
            if(logger_pb){
                logan_servers.emplace_back(std::move(logger_pb));
            }
        }
    }
    return logan_servers;
}

Experiment& Node::GetExperiment(){
    return experiment_;
}

bool Node::DeployedTo() const{
    return GetDeployedCount() > 0;
}