#include "environment.h"
#include "node.h"
#include "component.h"
#include "logger.h"
#include "ports/port.h"
#include "attribute.h"

using namespace EnvironmentManager;

Node::Node(Environment& environment, Experiment& parent, const NodeManager::Node& node) : 
            environment_(environment), experiment_(parent){
    
    name_ = node.info().name();
    id_ = node.info().id();

    for(const auto& attribute : node.attributes()){
        if(attribute.info().name() == "ip_address"){
            ip_ = attribute.s(0);
            break;
        }
    }

    for(const auto& logger : node.loggers()){
        AddLogger(logger);
    }

    for(const auto& component : node.components()){
        AddComponent(component);
    }
    
    for(const auto& attribute : node.attributes()){
        AddAttribute(attribute);
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
    return components_.size() + loggers_.size();
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

NodeManager::Node* Node::GetUpdate(){
    NodeManager::Node* node = 0;
    if(dirty_){
        node = new NodeManager::Node();
        node->mutable_info()->set_name(name_);
        node->mutable_info()->set_id(id_);
        node->set_type(NodeManager::Node::HARDWARE_NODE);

        auto ip_attribute = node->add_attributes();
        auto ip_attribute_info = ip_attribute->mutable_info();
        ip_attribute_info->set_name("ip_address");
        ip_attribute->add_s(ip_);

        auto management_port_attribute = node->add_attributes();
        auto management_port_attribute_info = management_port_attribute->mutable_info();
        management_port_attribute_info->set_name("management_port");
        management_port_attribute->add_s(management_port_);

        for(const auto& logger : loggers_){
            auto log_update = logger.second->GetUpdate();
            if(log_update){
                node->mutable_loggers()->AddAllocated(log_update);
            }
        }

        for(const auto& component : components_){
            auto component_update = component.second->GetUpdate();
            if(component_update){
                node->mutable_components()->AddAllocated(component_update);
            }
        }

        for(const auto& attribute : attributes_){
            auto attribute_update = attribute.second->GetUpdate();
            if(attribute_update){
                node->mutable_attributes()->AddAllocated(attribute_update);
            }
        }
        dirty_ = false;
    }
    return node;
}

NodeManager::Node* Node::GetProto(){
    auto node = new NodeManager::Node();
    
    node->mutable_info()->set_name(name_);
    node->mutable_info()->set_id(id_);
    node->set_type(NodeManager::Node::HARDWARE_NODE);

    auto ip_attribute = node->add_attributes();
    auto ip_attribute_info = ip_attribute->mutable_info();
    ip_attribute_info->set_name("ip_address");
    ip_attribute->add_s(ip_);

    auto management_port_attribute = node->add_attributes();
    auto management_port_attribute_info = management_port_attribute->mutable_info();
    management_port_attribute_info->set_name("management_port");
    management_port_attribute->add_s(management_port_);

    for(const auto& logger : loggers_){
        node->mutable_loggers()->AddAllocated(logger.second->GetProto());
    }

    for(const auto& component : components_){
        node->mutable_components()->AddAllocated(component.second->GetProto());
    }

    for(const auto& attribute : attributes_){
        node->mutable_attributes()->AddAllocated(attribute.second->GetProto());
    }

    return node;
}

NodeManager::EnvironmentMessage* Node::GetLoganDeploymentMessage() const{
    auto message = new NodeManager::EnvironmentMessage();

    for(const auto& logger_pair : loggers_){
        auto& logger = logger_pair.second;
        if(logger->GetType() == Logger::Type::Server){
            auto logger_pb = logger->GetProto();
            if(logger_pb){
                message->mutable_logger()->AddAllocated(logger_pb);
            }
        }
    }
    return message;

}

Experiment& Node::GetExperiment(){
    return experiment_;
}

bool Node::DeployedTo() const{
    return (loggers_.size() || components_.size());
}