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

std::string Node::GetId(){
    return id_;
};
std::string Node::GetName(){
    return name_;
};
std::string Node::GetIp(){
    return ip_;
};

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