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

std::string Port::GetId() const{
    return id_;
};
std::string Port::GetName() const{
    return name_;
};
Port::PortKind Port::GetKind() const{
    return kind_;
}
Port::PortMiddleware Port::GetMiddleware() const{
    return middleware_;
}
Component& Port::GetParent() const{
    return parent_;
}

void Port::SetTopic(const std::string& topic_name){
    topic_name_ = topic_name;
}

void Port::SetPublisherPort(const std::string& publisher_port){
    publisher_port_ = publisher_port;
}

std::string Port::GetPublisherPort() const{
    return publisher_port_;
}

std::string Port::GetTopic() const{
    return topic_name_;
}

void Port::AddEndpoint(const std::string& endpoint){
    endpoints_.insert(endpoint);
}

void Port::RemoveEndpoint(const std::string& endpoint){
    endpoints_.erase(endpoints_.find(endpoint));
}

void Port::AddConnectedPortId(const std::string& port_id){
    connected_port_ids_.insert(port_id);
}

void Port::AddExternalConnectedPortId(const std::string& port_id){
    connected_external_port_ids_.insert(port_id);
}

bool Port::IsConnectedTo(const std::string& port_id) const{
    return (connected_port_ids_.find(port_id) != connected_port_ids_.end()) || (connected_external_port_ids_.find(port_id) != connected_external_port_ids_.end());
}

void Port::SetDirty(){
    parent_.SetDirty();
    dirty_ = true;
}

bool Port::IsDirty() const{
    return dirty_;
}

NodeManager::Port* Port::GetUpdate(){
    NodeManager::Port* port;

    if(dirty_){
        port = new NodeManager::Port();
        port->info().set_name(name_);
        port->info().set_id(id_);
        port->info().set_type(type_);

        port->set_kind();
        port->set_middleware();

        for(const auto& connected_port : connected_port_ids_){
            port->add_connected_ports(connected_port);
        }

        for(const auto& external_port : connected_external_port_ids_){
            port->add_connected_ports(external_port);
        }

        for(const auto& attribute : attributes_){
            std::cout << "add attrs" << std::endl;
        }

        dirty_ = false;
    }
    return port;
}