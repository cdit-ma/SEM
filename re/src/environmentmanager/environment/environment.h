#ifndef ENVIRONMENT_MANAGER_ENVIRONMENT
#define ENVIRONMENT_MANAGER_ENVIRONMENT

#include "environmentmanager/endpointtracker.h"
#include "experiment.h"
#include "types/uniquequeue.hpp"
#include <experimentdefinition/experimentdefinition.h>
#include <iostream>
#include <mutex>
#include <proto/controlmessage/controlmessage.pb.h>
#include <string>
#include <types/ipv4.hpp>
#include <types/socketaddress.hpp>
#include <types/uuid.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <zmq/protowriter/protowriter.h>

namespace re::EnvironmentManager {

class Environment {
private:
    struct ExternalPort {
        std::string external_label;
        std::unordered_set<types::Uuid> producer_experiments;
        std::unordered_set<types::Uuid> consumer_experiments;
    };

public:
    enum class DeploymentType { EXECUTION_MASTER, LOGAN_SERVER };

    Environment(const types::Ipv4& ip_address,
                const types::SocketAddress& qpid_broker_address,
                std::string tao_naming_service_address,
                types::unique_queue<uint16_t> manager_port_pool,
                types::unique_queue<uint16_t> default_experiment_node_port_pool);
    ~Environment();

    types::Uuid GetExperimentUuid(const std::string& name);
    Experiment& GetExperiment(const types::Uuid& experiment_uuid);
    auto PopulateExperiment(NodeManagerRegistry& node_manager_registry,
                            const NodeManager::Experiment& message,
                            re::Representation::ExperimentDefinition experiment_definition) -> void;

    std::unique_ptr<NodeManager::RegisterExperimentReply>
    GetExperimentDeploymentInfo(const types::Uuid& experiment_uuid);

    auto GetDeploymentHandlerEndpoint(const types::Uuid& experiment_uuid,
                                      DeploymentType deployment_type) -> types::SocketAddress;
    auto GetUpdatePublisherEndpoint() const -> types::SocketAddress;

    void ShutdownExperiment(const types::Uuid& experiment_uuid);
    std::vector<types::Uuid> ShutdownExperimentRegex(const std::string& experiment_name_regex);

    void RemoveExperiment(const types::Uuid& experiment_uuid);

    std::unique_ptr<NodeManager::EnvironmentMessage>
    GetProto(const types::Uuid& experiment_uuid, bool full_update);

    bool ExperimentIsDirty(const types::Uuid& experiment_uuid);
    bool GotExperiment(const types::Uuid& experiment_uuid) const;

    bool IsExperimentRegistered(const types::Uuid& experiment_uuid);
    bool IsExperimentActive(const types::Uuid& experiment_uuid);
    bool IsExperimentConfigured(const types::Uuid& experiment_uuid);

    std::vector<std::string> GetExperimentNames() const;
    std::vector<std::unique_ptr<NodeManager::ExternalPort>> GetExternalPorts() const;

    bool NodeDeployedTo(const types::Uuid& experiment_uuid, const std::string& ip_address);

    auto GetExperimentManagerPublisherEndpoint(const types::Uuid& experiment_uuid)
        -> types::SocketAddress;
    auto GetExperimentManagerRegistrationEndpoint(const types::Uuid& experiment_uuid)
        -> types::SocketAddress;

    auto GetEndpoint(types::Ipv4 ip_address) -> types::SocketAddress;
    auto FreeEndpoint(types::SocketAddress address) -> void;

    auto GetManagerEndpoint() -> types::SocketAddress;
    auto FreeManagerEndpoint(types::SocketAddress) -> void;

    std::vector<std::reference_wrapper<Port>>
    GetExternalProducerPorts(const std::string& external_port_label);

    void AddExternalConsumerPort(const types::Uuid& experiment_uuid,
                                 const std::string& external_port_label);
    void AddExternalProducerPort(const types::Uuid& experiment_uuid,
                                 const std::string& external_port_label);

    void RemoveExternalConsumerPort(const types::Uuid& experiment_uuid,
                                    const std::string& external_port_label);
    void RemoveExternalProducerPort(const types::Uuid& experiment_uuid,
                                    const std::string& external_port_label);

    types::SocketAddress GetAmqpBrokerAddress();
    std::string GetTaoNamingServiceAddress();

    auto GetExperimentCount() -> size_t { return experiment_map_.size(); }
    auto CleanUpExperiments() -> void;

private:
    void ShutdownExperimentInternal(const types::Uuid& experiment_uuid);
    std::vector<types::Uuid> GetMatchingExperiments(const std::string& experiment_name_regex);

    std::string GetExperimentHandlerPort(const types::Uuid& experiment_uuid);

    void RemoveExperimentInternal(const types::Uuid& experiment_uuid);
    void DistributeContainerToImplicitNodeContainers(const types::Uuid& experiment_uuid,
                                                     const NodeManager::Container& container);
    void
    DeployContainer(const types::Uuid& experiment_uuid, const NodeManager::Container& container);

    void AddNodeToExperiment(const types::Uuid& experiment_uuid, const NodeManager::Node& node);
    void AddNodeToEnvironment(const NodeManager::Node& node);
    ExternalPort& GetExternalPort(const std::string& external_port_label);
    Experiment& GetExperimentInternal(const types::Uuid& experiment_uuid);
    void AddNodes(const types::Uuid& experiment_uuid, const NodeManager::Cluster& cluster);

    types::Ipv4 ip_address_;

    // ports available on the environment manager, uses same port range as nodes.
    EndpointTracker environment_manager_endpoint_pool_;

    types::SocketAddress qpid_broker_address_;
    // XXX: This may have some weird tao specific prefix.
    // TODO: Add tao endpoint type
    std::string tao_naming_service_address_;

    std::mutex configure_experiment_mutex_;

    // Review (Mitch): Why does this need to be mutable. Design error somewhere or just standard
    //  const fn evading bs?
    mutable std::mutex experiment_mutex_;
    // experiment_name -> experiment data structure
    std::unordered_map<types::Uuid, std::unique_ptr<EnvironmentManager::Experiment>> experiment_map_;

    std::mutex node_mutex_;
    // node_ip -> available endpoint data structure
    std::unordered_map<types::Ipv4, std::unique_ptr<EndpointTracker>> node_available_endpoint_map_;

    // node_name -> node_ip map
    std::unordered_map<std::string, types::Ipv4> node_ip_map_;

    // node_ip -> node_name map
    std::unordered_map<types::Ipv4, std::string> node_name_map_;

    // event port guid -> event port data structure
    // event port guid takes form
    // "experiment_id.{component_assembly_label}*n.component_instance_label.event_port_label"
    std::unordered_map<std::string, std::unique_ptr<ExternalPort>> external_eventport_map_;

    types::SocketAddress update_publisher_endpoint_;
    std::unique_ptr<::zmq::ProtoWriter> update_publisher_;

    // initially allocated unique_queue of port nums so we can copy into each node struct
    const types::unique_queue<uint16_t> default_experiment_node_port_pool_;
};
}; // namespace re::EnvironmentManager

#endif // ENVIRONMENT_MANAGER_ENVIRONMENT
