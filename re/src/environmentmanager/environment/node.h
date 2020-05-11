#ifndef ENVIRONMENT_MANAGER_NODE_H
#define ENVIRONMENT_MANAGER_NODE_H

#include <memory>
#include <mutex>
#include <unordered_map>

#include <experimentdefinition/experimentdefinition.h>
#include <proto/controlmessage/controlmessage.pb.h>
#include <types/ipv4.hpp>

namespace re::EnvironmentManager {

class Environment;
class Port;
class Experiment;
class Container;
class Logger;

class Node {
public:
    Node(Environment& environment, Experiment& experiment, const NodeManager::Node& node);

    // Info getters
    std::string GetId() const;
    std::string GetName() const;
    auto GetIp() const -> types::Ipv4;

    std::unique_ptr<NodeManager::HardwareId> GetHardwareId() const;
    std::vector<std::unique_ptr<NodeManager::ContainerId>> GetComponentContainerIds() const;

    // Node, component, attribute and logger adders
    void AddContainer(const NodeManager::Container& container);

    Container& AddContainer(const types::Uuid& event_uuid,
                            const re::Representation::ExperimentDefinition& experiment_definition,
                            const re::Representation::Container& container);

    void AddComponentToImplicitContainer(const NodeManager::Component& component);
    void AddLoggingClient(const NodeManager::Logger& logging_client);
    void AddLoggingServerToImplicitContainer(const NodeManager::Logger& logging_server);

    std::string GetMessage() const;

    // Update requirement management
    void SetDirty();
    bool IsDirty();

    Experiment& GetExperiment();

    // Deep getters
    bool HasContainer(const std::string& container_id) const;
    bool HasLogger(const std::string& logger_id) const;
    bool HasPort(const std::string& port_id) const;

    Container& GetContainer(const std::string& component_id) const;
    std::vector<std::reference_wrapper<Logger>> GetLoggers(const std::string& logger_id) const;
    Port& GetPort(const std::string& port_id) const;

    int GetDeployedCount() const;
    int GetDeployedComponentCount() const;
    int GetContainerCount() const;
    int GetLoganServerCount() const;

    std::vector<std::unique_ptr<NodeManager::Logger>> GetAllocatedLoganServers();

    // protobuf getters
    std::unique_ptr<NodeManager::Node> GetProto(bool full_update);
    auto GetContainers() -> const std::unordered_map<std::string, std::unique_ptr<Container>>&;

private:
    Environment& environment_;
    Experiment& experiment_;
    std::string id_;
    std::string name_;
    types::Ipv4 ip_;

    std::string implicit_container_id_;

    // Container id -> Container
    std::unordered_map<std::string, std::unique_ptr<Container>> containers_;

    bool dirty_;
};
}; // namespace re::EnvironmentManager

#endif // ENVIRONMENT_MANAGER_NODE_H