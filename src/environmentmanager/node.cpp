#include "environment.h"
#include "node.h"
#include "component.h"
#include "logger.h"
#include "port.h"
#include "attribute.h"

using namespace EnvironmentManager;

Node::Node(Environment& environment, Experiment& parent, const NodeManager::Node& node) : 
            environment_(environment), experiment_(parent){

    name_ = node.info().name();
    id_ = node.info().id();

    for(int i = 0; i < node.attributes_size(); i++){
        auto attribute = node.attributes(i);
        if(attribute.info().name() == "ip_address"){
            ip_ = attribute.s(0);
            break;
        }
    }

    for(int i = 0; i < node.loggers_size(); i++){
        AddLogger(node.loggers(i));
    }

    for(int i = 0; i < node.components_size(); i++){
        AddComponent(node.components(i));
    }

    for(int i = 0; i < node.attributes_size(); i++){
        AddAttribute(node.attributes(i));
    }
    //set logger port
    if(DeployedTo()){
        AddModelLogger();

        //set master/slave port
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

void Node::ConfigureConnections(){
    for(const auto& logger_pair : loggers_){
        logger_pair.second->ConfigureConnections();
    }
    for(const auto& component_pair : components_){
        component_pair.second->ConfigureConnections();
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
std::string Node::GetOrbPort(){
    if(!HasOrbPort()){
        orb_port_ = environment_.GetPort(ip_);
    }
    return orb_port_;
}

void Node::SetOrbPort(const std::string& orb_port){
    orb_port_ = orb_port;
}

void Node::AddComponent(const NodeManager::Component& component){
    components_.insert(std::make_pair(component.info().id(),
            std::unique_ptr<EnvironmentManager::Component>(
                new EnvironmentManager::Component(environment_, *this, component))));
}

void Node::AddLogger(const NodeManager::Logger& logger){
    loggers_.insert(std::make_pair(logger.id(), 
            std::unique_ptr<EnvironmentManager::Logger>(
                new EnvironmentManager::Logger(environment_, *this, logger))));
}


void Node::AddModelLogger(){
    loggers_.insert(std::make_pair("model_logger", 
            std::unique_ptr<EnvironmentManager::Logger>(new EnvironmentManager::Logger(environment_, *this, 
            EnvironmentManager::Logger::Type::Model, EnvironmentManager::Logger::Mode::Cached))));
}

void Node::AddAttribute(const NodeManager::Attribute& attribute){
    attributes_.insert(std::make_pair(attribute.info().id(), 
            std::unique_ptr<EnvironmentManager::Attribute>(
                new EnvironmentManager::Attribute(attribute))));
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
    NodeManager::Node* node;

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
            node->mutable_loggers()->AddAllocated(logger.second->GetUpdate());
        }

        for(const auto& component : components_){
            node->mutable_components()->AddAllocated(component.second->GetUpdate());
        }

        for(const auto& attribute : attributes_){
            node->mutable_attributes()->AddAllocated(attribute.second->GetUpdate());
        }

        dirty_ = false;
    }
    return node;
}

NodeManager::Node* Node::GetProto(){
    NodeManager::Node* node;

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
    NodeManager::EnvironmentMessage* message;
    message = new NodeManager::Logger();

    for(const auto& logger_pair : loggers_){
        auto& logger = logger_pair.second;
        if(logger->GetType() != Logger::Type::Model){
            message->mutable_logger()->AddAllocated(logger->GetDeploymentMessage());
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