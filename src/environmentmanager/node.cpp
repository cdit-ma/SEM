#include "node.h"
#include "component.h"
#include "loganclient.h"
#include "loganserver.h"

using namespace EnvironmentManager;

Node::Node(const std::string& id, const std::string& name, const std::string& ip_address){
    id_ = id;
    name_ = name;
    ip_ = ip_address;
    has_parent_ = false;
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

//Endpoint getters
std::string Node::GetManagementEndpoint() const{
    return management_endpoint_;
}
std::string Node::GetModelLoggerEndpoint() const {
    return model_logger_endpoint_;
}
std::string Node::GetOrbEndpoint() const {
    return model_logger_endpoint_;
}

//Port getters/setters
std::string Node::GetManagementPort() const{
    return management_port_;
}
std::string Node::GetModelLoggerPort() const {
    return model_logger_port_;
}
bool Node::HasOrbPort() const{
    return !orb_port_.empty();
}
std::string Node::GetOrbPort() const {
    return orb_port_;
}

void Node::AddNode(std::unique_ptr<Node> node){
    nodes_.insert({node.GetId(), std::move(node)});
}

void Node::AddComponent(std::unique_ptr<Component> component){
    components_.insert({component.GetId(), std::move(component)});
}

void Node::AddLogger(std::unique_ptr<LoganClient> logger){
    logan_servers_.insert({logger.GetId(), std::move(logger)});
}

void Node::AddLogger(std::unique_ptr<LoganServer> logger){
    logan_clients_.insert({logger.GetId(), std::move(logger)});
}

void Node::AddAttribute(){

}

void Node::SetDirty(){
    dirty_ = true;
}

bool Node::IsDirty(){
    return dirty_;
}

bool Node::HasNode(const std::string& node_id) const{
    return nodes_.count(id);
}

Node& Node::GetNode(const std::string& node_id) const{
    if(nodes_.count(node_id)){
        return nodes_.at(node_id);
    }

    throw std::out_of_range("Node::GetNode: " +id_ + " GET: "+ node_id);
}

bool Node::HasComponent(const std::string& component_id) const{
    return components_.count(component_id);
}

Component& Node::GetComponent(const std::string& component_id) const{
    if(components_.count(component_id)){
        return components_.at(component_id);
    }
    for(const auto& node : nodes_){
        if(node.HasComponent(component_id)){
            return node.GetComponent();
        }
    }

    throw std::out_of_range("Node::GetComponent: " + id_ + " GET: "+ component_id);
}

bool Node::HasPort(const std::string& port_id) const{
    for(const auto& component : components_){
        if(component.HasPort(port_id)){
            return true;
        }
    }
    return false;
}

Port& Node::GetPort(const std::string& port_id) const{
    for(const auto& component : components_){
        if(component.HasPort(port_id)){
            return component.GetPort(port_id);
        }
    }
}

std::vector<std::string> Node::GetAllPublisherPorts() const{
    std::vector<std::string> out;
    for(const auto& component : components_){
        auto component_port_list = component.GetAllPublisherPorts();
        out.insert(out.end(), component_port_list.begin(), component_port_list.end());
    }
    return out;
}

std::vector<std::string> Node::GetAllLoggingPorts() const{
    std::vector<std::string out;
    for(const auto& logger : loggers_){
        auto component_port_list = logger.GetPublisherPort();
        out.push_back(component_port_list);
    }
    return out;
}

NodeManager::Node* Node::GetUpdate(){
    NodeManager::Node* node;

    if(dirty_){
        node = new NodeManager::Node();
        node->info().set_name(name_);
        node->info().set_id(id_);
        node->set_type(NodeManager::Node::HARDWARE_NODE);

        auto ip_attribute = node->add_attributes();
        auto ip_attribute_info = ip_attribute->mutable_info()
        ip_attribute_info->set_name("ip_address");
        ip_attribute->add_s(ip_);

        auto ip_attribute = node->add_attributes();
        auto ip_attribute_info = ip_attribute->mutable_info()
        ip_attribute_info->set_name("modellogger_port");
        ip_attribute->add_s(model_logger_port_);

        auto ip_attribute = node->add_attributes();
        auto ip_attribute_info = ip_attribute->mutable_info()
        ip_attribute_info->set_name("management_port");
        ip_attribute->add_s(management_port_);

        for(const auto& component : components_){
            node->mutable_components()->AddAllocated(component.GetUpdate());
        }

        for(const auto& attribute : attributes_){
            std::cout << "fill attributes" << std::endl;
        }

        dirty_ = false;
    }
    return node;
}