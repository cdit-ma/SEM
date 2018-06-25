#include "component.h"
#include "node.h"
#include "port.h"

using namespace EnvironmentManager;

Component::Component(const Node& node, const std::string& id, const std::string& name, const std::string& type) : parent_(node){
    id_ = id;
    name_ = name;
    type_ = type;
}
std::string Component::GetId(){
    return id_;
};
std::string Component::GetName(){
    return name_;
};
Node& Component::GetParent(){
    return parent_;
}

void Component::AddPort(std::unique_ptr<Port> port){
    ports_.insert({port.GetId(), std::move(port)});
}

bool Component::SetDirty(){
    parent_.SetDirty();
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
        return ports_.at(port_id);
    }
    throw std::out_of_range("Component::GetPort: " + id_ + " Get: " + port_id);
}

std::vector<std::string> Component::GetAllPublisherPorts() const{
    std::vector<std::string> out;
    for(const auto& port : ports_){
        out.push_back(port.GetPublisherPort());
    }

    return out;
}

NodeManager::Component* Component::GetUpdate(){
    NodeManager::Component* component;
    if(dirty_){
        component = new NodeManager::Component();

        component->info().set_name(name_);
        component->info().set_id(id_);
        component->info().set_id(type_);

        for(const auto& port : ports_){
            component->mutable_components()->AddAllocated(port.GetUpdate());
        }

        for(const auto& attribute : attributes_){
            std::cout << "fill attributes" <<std::endl;
        }
    }
    return component;
}