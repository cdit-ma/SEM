#include "component.h"
#include "node.h"
#include "ports/port.h"
#include "attribute.h"
#include "worker.h"
#include "container.h"
#include "helper.h"

using namespace EnvironmentManager;

Component::Component(Environment& environment, Container& parent, const NodeManager::Component& component) :
                        environment_(environment), parent_(parent){
    id_ = component.info().id();
    name_ = component.info().name();
    type_ = component.info().type();

    //Append to the list of namespaces
    for(const auto& ns : component.info().namespaces()){
        namespaces_.emplace_back(ns);
    }

    for(const auto& location: component.location()){
        parent_stack_.push_back(location);
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
Container& Component::GetContainer() const{
    return parent_;
}

Node& Component::GetNode() const {
    return parent_.GetNode();
}

Environment& Component::GetEnvironment() const{
    return environment_;
}


void Component::SetDirty(){
    dirty_ = true;
    parent_.SetDirty();
}

bool Component::IsDirty(){
    return dirty_;
}

bool Component::HasPort(const std::string& port_id){
    return ports_.count(port_id) > 0;
}

Port& Component::GetPort(const std::string& port_id){
    if(ports_.count(port_id)){
        return *(ports_.at(port_id));
    }
    throw std::out_of_range("Component::GetPort: " + id_ + " Get: " + port_id);
}

std::unique_ptr<NodeManager::Component> Component::GetProto(const bool full_update){
    std::unique_ptr<NodeManager::Component> component;
    
    if(dirty_ || full_update){
        component = std::unique_ptr<NodeManager::Component>(new NodeManager::Component());

        component->mutable_info()->set_name(name_);
        component->mutable_info()->set_id(id_);
        component->mutable_info()->set_type(type_);

        for(const auto& ns : namespaces_){
            component->mutable_info()->add_namespaces(ns);
        }

        for(const auto& parent : parent_stack_){
            component->add_location(parent);
        }

        for(const auto& index : replication_indices_){
            component->add_replicate_indices(index);
        }

        for(const auto& port : ports_){
            auto port_pb = port.second->GetProto(full_update);
            if(port_pb){
                component->mutable_ports()->AddAllocated(port_pb.release());
            }
        }

        for(const auto& worker : workers_){
            auto worker_pb = worker.second->GetProto(full_update);
            if(worker_pb){
                component->mutable_workers()->AddAllocated(worker_pb.release());
            }
        }

        for(const auto& attribute : attributes_){
            auto attribute_pb = attribute.second->GetProto(full_update);

            if(attribute_pb){
                NodeManager::AddAllocatedAttribute(component->mutable_attributes(), std::move(attribute_pb));
            }
        }
        if(dirty_){
            dirty_ = false;
        }
    }
    return component;
}