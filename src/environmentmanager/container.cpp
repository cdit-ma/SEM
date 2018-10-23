#include "container.h"
#include "ports/port.h"
#include "node.h"
#include "component.h"
#include "logger.h"

using namespace EnvironmentManager;

Container::Container(EnvironmentManager::Environment &environment, Node &parent,
                                         const NodeManager::Container &container) :
        environment_(environment),
        parent_(parent)
{
    for(const auto& component : container.components()){
        AddComponent(component);
    }
    for(const auto& logger : container.loggers()){
        AddLogger(logger);
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
    return std::unique_ptr<NodeManager::Container>();
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

