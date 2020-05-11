#include "container.h"

#include "attribute.h"
#include "component.h"
#include "environment.h"
#include "logger.h"
#include "node.h"
#include "ports/port.h"
#include "worker.h"
#include <memory>

namespace re::EnvironmentManager {

Container::Container(EnvironmentManager::Environment& environment,
                     Node& parent,
                     const NodeManager::Container& container) :
    environment_(environment),
    parent_(parent),
    management_endpoint_{environment_.GetEndpoint(parent_.GetIp())},
    unqualified_orb_endpoint_{environment_.GetEndpoint(parent_.GetIp())}
{
    id_ = container.info().id();
    name_ = container.info().name();

    switch(container.type()) {
        case NodeManager::Container::GENERIC: {
            container_type_ = Type::Generic;
            break;
        }
        case NodeManager::Container::DOCKER: {
            container_type_ = Type::Docker;
            break;
        }
        default: {
            container_type_ = Type::Generic;
        }
    }

    late_joiner_ = container.is_late_joiner();

    for(const auto& component : container.components()) {
        AddComponent(component);
    }

    // If we have deployed components, add a model logger to this container
    if(GetDeployedComponentCount() > 0) {
        AddModelLogger();
    }

    // Add our explicitly defined loggers
    for(const auto& logger_pb : container.loggers()) {
        // Only Deploy Logging profiles if we have Components Deployed, or it's a server
        if(logger_pb.type() == NodeManager::Logger::SERVER || GetDeployedComponentCount() > 0) {
            AddLogger(logger_pb);
        }
    }
}

Container::Container(const types::Uuid& event_uuid,
                     Environment& environment,
                     Node& parent,
                     const re::Representation::ExperimentDefinition& experiment_definition,
                     const re::Representation::Container& container) :
    environment_(environment),
    parent_(parent),
    management_endpoint_{environment_.GetEndpoint(parent_.GetIp())},
    unqualified_orb_endpoint_{environment_.GetEndpoint(parent_.GetIp())}
{
    id_ = container.GetCoreData().GetMedeaId() + "_" + event_uuid.to_string();
    name_ = container.GetCoreData().GetMedeaName();

    switch(container.GetType()) {
        case Representation::Container::Type::Docker:
            container_type_ = Type::Docker;
        case Representation::Container::Type::OsProcess:
            container_type_ = Type::Generic;
    }

    for(const auto& component : experiment_definition.GetContainersComponents(container)) {
        AddComponent(event_uuid, experiment_definition, component);
    }

    AddModelLogger();
}

Container::~Container()
{
    // Free ports used for management and orb
    if(GetDeployedCount() > 0) {
        environment_.FreeEndpoint(management_endpoint_);
        environment_.FreeEndpoint(unqualified_orb_endpoint_);
    }
}

std::string Container::GetId()
{
    return id_;
}

std::string Container::GetName()
{
    return name_;
}

Component& Container::GetComponent(const std::string& component_id)
{
    return *(components_.at(component_id));
}

Environment& Container::GetEnvironment() const
{
    return environment_;
}

bool Container::HasComponent(const std::string& component_id)
{
    return components_.count(component_id) > 0;
}

bool Container::HasLogger(const std::string& logger_id)
{
    return loggers_.count(logger_id) > 0;
}

bool Container::HasPort(const std::string& port_id)
{
    for(const auto& component : components_) {
        if(component.second->HasPort(port_id)) {
            return true;
        }
    }
    return false;
}

bool Container::IsDocker() const
{
    return container_type_ == Type::Docker;
}

Port& Container::GetPort(const std::string& port_id)
{
    for(const auto& component : components_) {
        if(component.second->HasPort(port_id)) {
            return component.second->GetPort(port_id);
        }
    }
    throw std::out_of_range("Container::GetPort: " + id_ + " Get: " + port_id);
}

void Container::SetDirty()
{
    dirty_ = true;
    parent_.SetDirty();
}

bool Container::IsDirty()
{
    return dirty_;
}

std::unique_ptr<NodeManager::Container> Container::GetProto(const bool full_update)
{
    std::unique_ptr<NodeManager::Container> container;

    if(dirty_ || full_update) {
        container = std::make_unique<NodeManager::Container>();

        container->mutable_info()->set_name(name_);
        container->mutable_info()->set_id(id_);
        container->mutable_info()->set_type(type_);

        if(container_type_ == Type::Generic) {
            container->set_type(NodeManager::Container::GENERIC);
        }
        if(container_type_ == Type::Docker) {
            container->set_type(NodeManager::Container::DOCKER);
        }

        for(const auto& component : components_) {
            auto component_pb = component.second->GetProto(full_update);
            if(component_pb) {
                container->mutable_components()->AddAllocated(component_pb.release());
            }
        }

        for(const auto& logger : loggers_) {
            auto logger_pb = logger.second->GetProto(full_update);
            if(logger_pb) {
                container->mutable_loggers()->AddAllocated(logger_pb.release());
            }
        }

        if(dirty_) {
            dirty_ = false;
        }
    }
    return container;
}

void Container::AddComponent(const NodeManager::Component& component_pb)
{
    const auto& id = component_pb.info().id();
    auto component = std::make_unique<Component>(environment_, *this, component_pb);
    components_.emplace(id, std::move(component));
}

void Container::AddComponent(const types::Uuid& event_uuid,
                             const re::Representation::ExperimentDefinition& experiment_definition,
                             const re::Representation::ComponentInstance& component)
{
    auto new_component = std::make_unique<Component>(event_uuid, environment_, *this,
                                                     experiment_definition, component);
    auto id = new_component->GetId();
    components_.emplace(id, std::move(new_component));
}

void Container::AddLogger(const NodeManager::Logger& logger_pb)
{
    const auto& id = logger_pb.id();
    loggers_.emplace(id, std::make_unique<Logger>(environment_, *this, logger_pb));
    const auto& logger = GetLogger(id);

    if(logger.GetType() == Logger::Type::Client) {
        try {
            // Set model_logger mode to be the mode of the last client we discover
            auto& model_logger = GetModelLogger();
            model_logger.SetMode(logger.GetMode());
        } catch(const std::exception&) {
        }
    }
}

int Container::GetDeployedComponentCount() const
{
    return components_.size();
}

int Container::GetDeployedCount() const
{
    return loggers_.size() + components_.size();
}

Node& Container::GetNode() const
{
    return parent_;
}

int Container::GetLoganServerCount() const
{
    int temp = 0;
    for(const auto& logger : loggers_) {
        if(logger.second->GetType() == Logger::Type::Server) {
            temp++;
        }
    }
    return temp;
}

auto Container::GetUnqualifiedOrbEndpoint() const -> types::SocketAddress
{
    return unqualified_orb_endpoint_;
}

void Container::AddModelLogger()
{
    loggers_.emplace(Logger::MODEL_LOGGER_ID,
                     std::make_unique<Logger>(environment_, *this, Logger::Type::Model,
                                              Logger::Mode::Off));
}

EnvironmentManager::Logger& Container::GetModelLogger()
{
    return GetLogger(Logger::MODEL_LOGGER_ID);
}

EnvironmentManager::Logger& Container::GetExperimentLogger()
{
    return GetLogger(Logger::EXPERIMENT_LOGGER_ID);
}

Logger& Container::GetLogger(const std::string& logger_id)
{
    return *(loggers_.at(logger_id));
}

bool Container::IsLateJoiner() const
{
    return late_joiner_;
}

std::vector<std::string> Container::GetLoganServerIds() const
{
    std::vector<std::string> server_ids;
    for(const auto& logger : loggers_) {
        if(logger.second->GetType() == Logger::Type::Server) {
            server_ids.emplace_back(logger.second->GetId());
        }
    }
    return server_ids;
}

} // namespace re::EnvironmentManager