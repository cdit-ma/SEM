#include "component.h"
#include "node.h"
#include "port.h"
#include "attribute.h"

using namespace EnvironmentManager;

Component::Component(Environment& environment, Node& parent, const NodeManager::Component& component) : 
                        environment_(environment), node_(parent){
    id_ = component.info().id();
    name_ = component.info().name();
    type_ = component.info().type();

    for(int i = 0; i < component.ports_size(); i++){
        ports_.insert(std::make_pair(component.ports(i).info().id(), 
            std::unique_ptr<EnvironmentManager::Port>(
                new EnvironmentManager::Port(environment_, *this, component.ports(i)))));
    }


    //Add attributes
    for(int i = 0; i < component.attributes_size(); i++){
        attributes_.insert(std::make_pair(component.attributes(i).info().id(), 
            std::unique_ptr<EnvironmentManager::Attribute>(
                new EnvironmentManager::Attribute(component.attributes(i)))));
    }
    //add workers
}

void Component::ConfigureConnections(){
    for(auto& port_pair : ports_){
        port_pair.second->ConfigureConnections();
    }
}

std::string Component::GetId(){
    return id_;
};
std::string Component::GetName(){
    return name_;
};
Node& Component::GetNode(){
    return node_;
}

void Component::AddPort(const NodeManager::Port& port){
    ports_.insert(std::make_pair(port.info().id(), 
            std::unique_ptr<EnvironmentManager::Port>(
                new EnvironmentManager::Port(environment_, *this, port))));
}

void Component::SetDirty(){
    node_.SetDirty();
    dirty_ = true;
}

bool Component::IsDirty(){
    return dirty_;
}

bool Component::HasPort(const std::string& port_id){
    return ports_.count(port_id);
}

Port& Component::GetPort(const std::string& port_id){
    if(ports_.count(port_id)){
        return *(ports_.at(port_id));
    }
    throw std::out_of_range("Component::GetPort: " + id_ + " Get: " + port_id);
}

std::vector<std::string> Component::GetAllPublisherPorts() const{
    std::vector<std::string> out;
    for(const auto& port : ports_){
        out.push_back(port.second->GetPublisherPort());
    }

    return out;
}

NodeManager::Component* Component::GetUpdate(){
    NodeManager::Component* component;
    if(dirty_){
        component = new NodeManager::Component();

        component->mutable_info()->set_name(name_);
        component->mutable_info()->set_id(id_);
        component->mutable_info()->set_type(type_);

        for(const auto& port : ports_){
            component->mutable_ports()->AddAllocated(port.second->GetUpdate());
        }

        for(const auto& attribute : attributes_){
            component->mutable_attributes()->AddAllocated(attribute.second->GetProto());
        }
        dirty_ = false;
    }
    return component;
}

NodeManager::Component* Component::GetProto(){
    NodeManager::Component* component;
    component = new NodeManager::Component();

    component->mutable_info()->set_name(name_);
    component->mutable_info()->set_id(id_);
    component->mutable_info()->set_type(type_);

    for(const auto& port : ports_){
        component->mutable_ports()->AddAllocated(port.second->GetProto());
    }

    for(const auto& attribute : attributes_){
        component->mutable_attributes()->AddAllocated(attribute.second->GetProto());
    }
    return component;
}