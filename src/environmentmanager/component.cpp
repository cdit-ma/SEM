#include "component.h"
#include "node.h"
#include "ports/port.h"
#include "attribute.h"
#include "worker.h"
#include <proto/controlmessage/helper.h>

using namespace EnvironmentManager;

Component::Component(Environment& environment, Node& parent, const NodeManager::Component& component) : 
                        environment_(environment), node_(parent){
    id_ = component.info().id();
    name_ = component.info().name();
    type_ = component.info().type();

    //Append to the list of namespaces
    for(const auto& ns : component.info().namespaces()){
        namespaces_.emplace_back(ns);
    }

    for(const auto& parent: component.location()){
        parent_stack_.push_back(parent);
    }

    for(const auto& index: component.replicate_indices()){
        replication_indices_.push_back(index);
    }

    for(const auto& port_pb : component.ports()){
        const auto& id = port_pb.info().id();
        auto port = Port::ConstructPort(*this, port_pb);
        if(port){
            ports_.emplace(id, std::move(port));
        }
    }

    for(const auto& worker_pb : component.workers()){
        const auto& id = worker_pb.info().id();
        workers_.emplace(id, std::unique_ptr<Worker>(new Worker(*this, worker_pb)));
    }

    for(const auto& pair : component.attributes()){
        const auto& id = pair.first;
        const auto& attr_pb = pair.second;
        auto attr = std::unique_ptr<EnvironmentManager::Attribute>(new EnvironmentManager::Attribute(attr_pb));
        attributes_.emplace(id, std::move(attr));
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

Environment& Component::GetEnvironment() const{
    return environment_;
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

NodeManager::Component* Component::GetUpdate(){
    if(dirty_){
        auto component = new NodeManager::Component();
        component->mutable_info()->set_name(name_);
        component->mutable_info()->set_id(id_);
        component->mutable_info()->set_type(type_);

        for(const auto& ns : namespaces_){
            component->mutable_info()->add_namespaces(ns);
        }

        for(const auto& port : ports_){
            auto port_update = port.second->GetUpdate();
            if(port_update){
                component->mutable_ports()->AddAllocated(port_update);
            }
        }

        for(const auto& worker : workers_){
            auto worker_update = worker.second->GetUpdate();
            if(worker_update){
                component->mutable_workers()->AddAllocated(worker_update);
            }
        }

        for(const auto& attribute : attributes_){
            auto attribute_proto = attribute.second->GetProto();

            if(attribute_proto){
                (*component->mutable_attributes())[attribute.first] = *attribute_proto;
                delete attribute_proto;
            }
        }
        dirty_ = false;
        return component;
    }
    return nullptr;
}

NodeManager::Component* Component::GetProto(){
    auto component = new NodeManager::Component();

    component->mutable_info()->set_name(name_);
    component->mutable_info()->set_id(id_);
    component->mutable_info()->set_type(type_);

    for(const auto& parent : parent_stack_){
        component->add_location(parent);
    }

    for(const auto& index : replication_indices_){
        component->add_replicate_indices(index);
    }

    for(const auto& port : ports_){
        component->mutable_ports()->AddAllocated(port.second->GetProto());
    }

    for(const auto& worker : workers_){
        component->mutable_workers()->AddAllocated(worker.second->GetProto());
    }

    for(const auto& attribute : attributes_){
        NodeManager::AddAllocatedAttribute(component->mutable_attributes(), attribute.second->GetProto());
    }

    return component;
}