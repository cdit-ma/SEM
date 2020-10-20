#ifndef ENVIRONMENTMANAGER_CONTAINER_H
#define ENVIRONMENTMANAGER_CONTAINER_H

#include "uniquequeue.hpp"
#include "container.h"
#include "experimentdefinition.h"
#include <memory>
#include "socketaddress.hpp"
#include <unordered_map>
#include "controlmessage.pb.h"

namespace re::EnvironmentManager {
class Environment;
class Node;
class Component;
class Port;
class Logger;

class Container {
public:
    enum class Type { Docker, Generic };
    Container(Environment& environment, Node& parent, const NodeManager::Container& container);
    Container(const sem::types::Uuid& event_uuid,
              Environment& environment,
              Node& parent,
              const re::Representation::ExperimentDefinition& experiment_definition,
              const re::Representation::Container& container);
    ~Container();

    std::string GetId();
    std::string GetName();

    Environment& GetEnvironment() const;
    Node& GetNode() const;

    bool HasComponent(const std::string& component_id);
    bool HasLogger(const std::string& logger_id);
    bool HasPort(const std::string& port_id);

    Component& GetComponent(const std::string& component_id);
    Logger& GetLogger(const std::string& logger_id);
    Logger& GetExperimentLogger();
    Port& GetPort(const std::string& port_id);

    void AddComponent(const NodeManager::Component& component_pb);
    void AddComponent(const sem::types::Uuid& event_uuid,
                      const re::Representation::ExperimentDefinition& experiment_definition,
                      const re::Representation::ComponentInstance& component);
    void AddLogger(const NodeManager::Logger& logger_pb);

    int GetDeployedCount() const;
    int GetDeployedComponentCount() const;
    int GetLoganServerCount() const;
    std::vector<std::string> GetLoganServerIds() const;

    auto GetUnqualifiedOrbEndpoint() const -> sem::types::SocketAddress;

    bool IsDocker() const;

    bool IsLateJoiner() const;

    void SetDirty();
    bool IsDirty();

    std::unique_ptr<NodeManager::Container> GetProto(bool full_update);

private:
    void AddModelLogger();
    EnvironmentManager::Logger& GetModelLogger();

    Environment& environment_;
    Node& parent_;

    std::string id_;
    std::string name_;
    std::string type_;
    Type container_type_;

    sem::types::SocketAddress management_endpoint_;
    sem::types::SocketAddress unqualified_orb_endpoint_;

    bool dirty_;

    bool late_joiner_;

    std::unordered_map<std::string, std::unique_ptr<Component>> components_;
    std::unordered_map<std::string, std::unique_ptr<Logger>> loggers_;
};
} // namespace re::EnvironmentManager

#endif // PROJECT_CONTAINER_H
