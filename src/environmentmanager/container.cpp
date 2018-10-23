#include "container.h"
#include "ports/port.h"
#include "node.h"
#include "component.h"
#include "worker.h"
#include "attribute.h"
#include "logger.h"
#include "environment.h"

using namespace EnvironmentManager;

Container::Container(EnvironmentManager::Environment &environment, Node &parent,
                                         const NodeManager::Container &container) :
        environment_(environment),
        parent_(parent)
{
    for(const auto& component : container.components()){
        AddComponent(component);
    }

    // If we have deployed components, add a model logger to this container
    if(!components_.empty()){
        AddModelLogger();
    }

    // Add our explicitly defined loggers
    for(const auto& logger : container.loggers()){
        AddLogger(logger);
    }
}

Container::~Container() {
    // Free ports used for management and orb
    if(GetDeployedCount() > 0) {
        environment_.FreePort(GetNode().GetIp(), management_port_);

        if(HasOrbPort()) {
            environment_.FreePort(GetNode().GetIp(), orb_port_);
        }
    }
}

std::string Container::GetId() {
    return id_;
}

std::string Container::GetName() {
    return name_;
}

Component& Container::GetComponent(const std::string &component_id) {
    return *(components_.at(component_id));
}

Environment& Container::GetEnvironment() const {
    return environment_;
}

bool Container::HasComponent(const std::string &component_id) {
    return components_.count(component_id) > 0;
}

bool Container::HasLogger(const std::string &logger_id) {
    return loggers_.count(logger_id) > 0;
}

bool Container::HasPort(const std::string &port_id) {
    for (const auto &component : components_) {
        if(component.second->HasPort(port_id)){
            return true;
        }
    }
    return false;
}

Port &Container::GetPort(const std::string &port_id) {
    for (const auto &component : components_) {
        if (component.second->HasPort(port_id)) {
            return component.second->GetPort(port_id);
        }
    }
    throw std::out_of_range("Container::GetPort: " + id_ + " Get: " + port_id);
}

void Container::SetDirty() {

}

bool Container::IsDirty() {
    return false;
}

std::unique_ptr<NodeManager::Container> Container::GetProto(const bool full_update) {
    std::unique_ptr<NodeManager::Container> container;

    if(dirty_ || full_update){
        container = std::unique_ptr<NodeManager::Container>(new NodeManager::Container());

        container->mutable_info()->set_name(name_);
        container->mutable_info()->set_id(id_);
        container->mutable_info()->set_type(type_);

        for(const auto& component : components_) {
            auto component_pb = component.second->GetProto(full_update);
            container->mutable_components()->AddAllocated(component_pb.release());
        }

        for(const auto& logger : loggers_) {
            auto logger_pb = logger.second->GetProto(full_update);
            container->mutable_loggers()->AddAllocated(logger_pb.release());
        }

        if(dirty_){
            dirty_ = false;
        }
    }
    return container;
}

void Container::AddComponent(const NodeManager::Component &component_pb) {
    const auto& id = component_pb.info().id();
    auto component = std::unique_ptr<Component>(new Component(environment_, *this, component_pb));
    components_.emplace(id, std::move(component));
}

void Container::AddLogger(const NodeManager::Logger &logger_pb) {
    const auto& id = logger_pb.id();
    loggers_.emplace(id, std::move(std::unique_ptr<Logger>(new Logger(environment_, *this, logger_pb))));

}

int Container::GetDeployedComponentCount() const {
    return components_.size();
}

int Container::GetDeployedCount() const {
    return loggers_.size() + components_.size();
}

Node& Container::GetNode() const {
    return parent_;
}

int Container::GetLoganServerCount() const {
    int temp = 0;
    for(const auto& logger : loggers_) {
        if(logger.second->GetType() == Logger::Type::Server){
            temp ++;
        }
    }
    return temp;
}

bool Container::HasOrbPort() const {
    return !orb_port_.empty();
}

std::string Container::GetOrbPort() const {
    return orb_port_;
}

std::string Container::AssignOrbPort() {
    if(!HasOrbPort()) {
        orb_port_ = environment_.GetPort(GetNode().GetIp());
    }
    return orb_port_;
}

void Container::SetOrbPort(const std::string& orb_port) {
    orb_port_ = orb_port;
}

void Container::AddModelLogger() {
    auto id = "model_logger";
    loggers_.emplace(id, std::unique_ptr<Logger>(new Logger(environment_, *this, Logger::Type::Model, Logger::Mode::Cached)));
}

Logger &Container::GetLogger(const std::string &logger_id) {
    return *(loggers_.at(logger_id));
}

