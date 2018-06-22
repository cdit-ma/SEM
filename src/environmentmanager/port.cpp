#include "component.h"
#include "port.h"

using namespace EnvironmentManager;

Port::Port(const Component& parent, const std::string& id, const std::string& name, Port::PortKind kind, Port::PortMiddleware middleware) :
            parent_(parent){
    id_ = id;
    name_ = name;
    kind_ = kind;
    middleware_ = middleware;
}

std::string Port::GetId(){
    return id_;
};
std::string Port::GetName(){
    return name_;
};
Port::PortKind Port::GetKind(){
    return kind_;
}
Port::PortMiddleware Port::GetMiddleware(){
    return middleware_;
}
Component& Port::GetParent(){
    return parent_;
}

void Port::SetTopic(const std::string& topic_name){
    topic_name_ = topic_name;
}

void Port::AddEndpoint(const std::string& endpoint){
    endpoints_.push_back(endpoint);
}

void Port::AddConnectedPortId(const std::string& port_id){
    connected_port_ids_.insert(port_id);
}

void Port::AddExternalConnectedPortId(const std::string& port_id){
    connected_external_port_ids_.insert(port_id);
}

bool Port::IsConnectedTo(const std::string& port_id){
    return connected_port_ids_.contains(port_id) || connected_external_port_ids_.contains(port_id);
}

void Port::SetDirty(){
    parent_.SetDirty();
    dirty_ = true;
}

bool Port::IsDirty(){
    return dirty_;
}