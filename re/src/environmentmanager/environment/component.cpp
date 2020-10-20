#include "component.h"
#include "attribute.h"
#include "container.h"
#include "node.h"
#include "ports/port.h"
#include "triggerprototype.h"
#include "worker.h"
#include "helper.h"

#include "ports/periodicport.h"
#include <memory>

namespace re::EnvironmentManager {

Component::Component(Environment& environment,
                     Container& parent,
                     const NodeManager::Component& component) :
    environment_(environment), parent_(parent)
{
    id_ = component.info().id();
    name_ = component.info().name();
    type_ = component.info().type();

    // Append to the list of namespaces
    for(const auto& ns : component.info().namespaces()) {
        namespaces_.emplace_back(ns);
    }

    for(const auto& location : component.location()) {
        parent_stack_.push_back(location);
    }

    for(const auto& index : component.replicate_indices()) {
        replication_indices_.push_back(index);
    }

    for(const auto& port_pb : component.ports()) {
        const auto& id = port_pb.info().id();
        auto port = Port::ConstructPort(*this, port_pb);
        if(port) {
            ports_.emplace(id, std::move(port));
        }
    }

    for(const auto& worker_pb : component.workers()) {
        const auto& id = worker_pb.info().id();
        workers_.emplace(id, std::make_unique<Worker>(*this, worker_pb));
    }

    for(const auto& trigger_pb : component.prototype_triggers()) {
        const auto& id = trigger_pb.info().id();
        triggers_.emplace(id, std::make_unique<TriggerPrototype>(environment, *this, trigger_pb));
    }

    for(const auto& pair : component.attributes()) {
        const auto& id = pair.first;
        const auto& attr_pb = pair.second;
        auto attr = std::make_unique<EnvironmentManager::Attribute>(attr_pb);
        attributes_.emplace(id, std::move(attr));
    }
}

Component::Component(const types::Uuid& event_uuid,
                     Environment& environment,
                     Container& parent,
                     const Representation::ExperimentDefinition& experiment_definition,
                     const Representation::ComponentInstance& component_instance) :
    environment_(environment), parent_(parent)
{
    id_ = component_instance.GetCoreData().GetMedeaId() + "_" + event_uuid.to_string();
    name_ = component_instance.GetCoreData().GetMedeaName();
    type_ = component_instance.GetMedeaType();

    for(const auto& port_inst :
        experiment_definition.GetComponentInstanceMiddlewarePorts(component_instance)) {
        auto port = Port::ConstructPort(event_uuid, *this, experiment_definition, port_inst);
        if(port) {
            ports_.emplace(port->GetId(), std::move(port));
        }
    }

    for(const auto& periodic_port_inst :
        experiment_definition.GetComponentInstancePeriodicPorts(component_instance)) {
        auto port = std::unique_ptr<Port>(
            new PeriodicPort(event_uuid, *this, experiment_definition, periodic_port_inst));
        if(port) {
            ports_.emplace(port->GetId(), std::move(port));
        }
    }

    for(const auto& worker_inst :
        experiment_definition.GetComponentInstanceWorkers(component_instance)) {
        auto worker = std::make_unique<Worker>(event_uuid, *this, experiment_definition,
                                               worker_inst);
        const auto& id = worker->GetId();
        workers_.emplace(id, std::move(worker));
    }

    for(const auto& attribute_inst :
        experiment_definition.GetComponentAttributeInstances(component_instance)) {

        auto attr = std::make_unique<EnvironmentManager::Attribute>(event_uuid, *attribute_inst);
        auto id = attr->GetId();
        attributes_.emplace(id, std::move(attr));

    }

    //
    //    for(const auto& trigger_inst :
    //        experiment_definition.GetComponentInstanceTriggers(component_instance)) {
    //        const auto& id = trigger_pb.info().id();
    //        triggers_.emplace(id, std::make_unique<TriggerPrototype>(environment, *this,
    //        trigger_pb));
    //    }
}

std::string Component::GetId()
{
    return id_;
};
std::string Component::GetName()
{
    return name_;
};
Container& Component::GetContainer() const
{
    return parent_;
}

Node& Component::GetNode() const
{
    return parent_.GetNode();
}

Environment& Component::GetEnvironment() const
{
    return environment_;
}

void Component::SetDirty()
{
    dirty_ = true;
    parent_.SetDirty();
}

bool Component::IsDirty()
{
    return dirty_;
}

bool Component::HasPort(const std::string& port_id)
{
    return ports_.count(port_id) > 0;
}

Port& Component::GetPort(const std::string& port_id)
{
    if(ports_.count(port_id)) {
        return *(ports_.at(port_id));
    }
    throw std::out_of_range("Component::GetPort: " + id_ + " Get: " + port_id);
}

std::unique_ptr<NodeManager::Component> Component::GetProto(const bool full_update)
{
    std::unique_ptr<NodeManager::Component> component;

    if(dirty_ || full_update) {
        component = std::make_unique<NodeManager::Component>();

        component->mutable_info()->set_name(name_);
        component->mutable_info()->set_id(id_);
        component->mutable_info()->set_type(type_);

        for(const auto& ns : namespaces_) {
            component->mutable_info()->add_namespaces(ns);
        }

        for(const auto& parent : parent_stack_) {
            component->add_location(parent);
        }

        for(const auto& index : replication_indices_) {
            component->add_replicate_indices(index);
        }

        for(const auto& port : ports_) {
            auto port_pb = port.second->GetProto(full_update);
            if(port_pb) {
                component->mutable_ports()->AddAllocated(port_pb.release());
            }
        }

        for(const auto& worker : workers_) {
            auto worker_pb = worker.second->GetProto(full_update);
            if(worker_pb) {
                component->mutable_workers()->AddAllocated(worker_pb.release());
            }
        }

        for(const auto& attribute : attributes_) {
            auto attribute_pb = attribute.second->GetProto(full_update);

            if(attribute_pb) {
                NodeManager::AddAllocatedAttribute(component->mutable_attributes(),
                                                   std::move(attribute_pb));
            }
        }

        for(const auto& trigger : triggers_) {
            auto trigger_pb = trigger.second->GetProto(full_update);

            if(trigger_pb) {
                component->mutable_prototype_triggers()->AddAllocated(trigger_pb.release());
            }
        }

        if(dirty_) {
            dirty_ = false;
        }
    }
    return component;
}

} // namespace re::EnvironmentManager