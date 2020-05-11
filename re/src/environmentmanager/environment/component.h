#ifndef ENVIRONMENT_MANAGER_COMPONENT_H
#define ENVIRONMENT_MANAGER_COMPONENT_H

#include "types/uniquequeue.hpp"
#include <experimentdefinition/experimentdefinition.h>
#include <memory>
#include <mutex>
#include <proto/controlmessage/controlmessage.pb.h>
#include <unordered_map>

namespace re::EnvironmentManager {
class Environment;
class Node;
class Port;
class Attribute;
class Container;
class Worker;
class TriggerPrototype;

class Component {
public:
    Component(Environment& environment, Container& parent, const NodeManager::Component& component);
    Component(const types::Uuid& event_uuid,
              Environment& environment,
              Container& parent,
              const re::Representation::ExperimentDefinition& experiment_definition,
              const re::Representation::ComponentInstance& component_instance);
    void AddAttribute();

    std::string GetId();
    std::string GetName();

    Port& GetPort(const std::string& port_id);
    Container& GetContainer() const;
    Node& GetNode() const;
    Environment& GetEnvironment() const;

    bool HasPort(const std::string& port_id);

    void SetDirty();
    bool IsDirty();

    std::unique_ptr<NodeManager::Component> GetProto(const bool full_update);

private:
    Environment& environment_;
    Container& parent_;
    std::string id_;
    std::string name_;
    std::string type_;
    std::vector<std::string> namespaces_;

    bool dirty_;

    std::unordered_map<std::string, std::unique_ptr<Port>> ports_;
    std::unordered_map<std::string, std::unique_ptr<Worker>> workers_;
    std::unordered_map<std::string, std::unique_ptr<Attribute>> attributes_;
    std::unordered_map<std::string, std::unique_ptr<TriggerPrototype>> triggers_;

    std::vector<std::string> parent_stack_;
    std::vector<int> replication_indices_;
};
}; // namespace re::EnvironmentManager

#endif // ENVIRONMENT_MANAGER_COMPONENT_H