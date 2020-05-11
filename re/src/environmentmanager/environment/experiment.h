#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "ports/port.h"
#include "triggerprototype.h"
#include "types/uniquequeue.hpp"
#include <chrono>
#include <mutex>
#include <proto/controlmessage/controlmessage.pb.h>
#include <types/socketaddress.hpp>
#include <types/uuid.h>
#include <unordered_map>

#include <DockerClient/docker.h>
#include <environmentmanager/nodemanagerregistry.h>
#include <experimentdefinition/experimentdefinition.h>
#include <network/protocols/epmregistration/epmregistration.pb.h>
#include <network/protocols/triggerevent/triggerevent.pb.h>
#include <network/subscriber.hpp>
#include <types/timeout.h>
namespace re::EnvironmentManager {
class Environment;
class Node;
class Port;
class Logger;
class Container;
class ExperimentManager;

class Experiment {
public:
    struct ExternalPort {
        enum class Kind { PubSub, ReqRep };

        std::string external_label;
        std::string internal_id;

        std::string type;

        Port::Middleware middleware;
        Kind kind;

        std::set<std::string> producer_ids;
        std::set<std::string> consumer_ids;
    };

    enum class ExperimentState { REGISTERED, CONFIGURED, ACTIVE, FINISHED };
    Experiment(Environment& environment,
               NodeManagerRegistry& node_manager_registry,
               types::Uuid uuid,
               std::string name,
               re::Representation::ExperimentDefinition experiment_definition);
    ~Experiment();

    ExperimentState GetState() const;

    void SetConfigured();
    void SetActive();
    void SetFinished();

    void SetDuration(const types::Timeout& duration);

    bool IsRegistered() const;
    bool IsConfigured() const;
    bool IsActive() const;
    bool IsFinished() const;

    void CheckValidity() const;

    const std::string& GetName() const;
    auto GetUuid() const -> types::Uuid { return experiment_uuid_; };

    std::string GetMessage() const;

    void AddExternalPorts(const NodeManager::Experiment& message);
    void AddNode(const NodeManager::Node& node);
    void AddLoggingClientToNodes(const NodeManager::Logger& logging_client);

    Node& GetNode(const std::string& ip_address) const;

    void Shutdown();

    bool HasDeploymentOn(const std::string& node_name) const;

    Environment& GetEnvironment() const;

    std::unique_ptr<NodeManager::RegisterExperimentReply> GetDeploymentInfo();
    std::vector<std::unique_ptr<NodeManager::Logger>>
    GetAllocatedLoganServers(const std::string& ip_address);
    std::unique_ptr<NodeManager::EnvironmentMessage> GetProto(bool full_update);

    types::SocketAddress GetExperimentManagerPublisherEndpoint();
    types::SocketAddress GetExperimentManagerRegistrationEndpoint();
    types::SocketAddress GetExperimentLoggerEndpoint();

    Node& GetLeastDeployedToNode(bool non_empty = false);
    Port& GetPort(const std::string& id);

    bool IsDirty() const;
    void SetDirty();

    void UpdatePort(const std::string& external_port_label);

    std::vector<std::reference_wrapper<Port>>
    GetExternalProducerPorts(const std::string& external_port_label);
    std::vector<std::reference_wrapper<Logger>> GetLoggerClients(const std::string& logger_id);

    std::string GetExternalPortLabel(const std::string& internal_port_id);
    std::string GetExternalPortInternalId(const std::string& external_port_label);

    auto StartExperimentManager() -> void;
    auto StartExperimentProcesses() -> void;

    void AddExternalConsumerPort(const std::string& external_port_internal_id,
                                 const std::string& internal_port_id);
    void AddExternalProducerPort(const std::string& external_port_internal_id,
                                 const std::string& internal_port_id);
    void RemoveExternalConsumerPort(const std::string& external_port_internal_id,
                                    const std::string& internal_port_id);
    void RemoveExternalProducerPort(const std::string& external_port_internal_id,
                                    const std::string& internal_port_id);

    std::vector<std::reference_wrapper<ExternalPort>> GetExternalPorts() const;

private:
    using EpmRegistrationRequest = network::protocol::epmregistration::Request;
    using EpmRegistrationReply = network::protocol::epmregistration::Reply;

    using TriggerEvent = network::protocol::triggerevent::Event;

    ExternalPort& GetExternalPort(const std::string& external_port_internal_id);

    mutable std::mutex mutex_;

    Environment& environment_;
    NodeManagerRegistry& node_manager_registry_;

    re::Representation::ExperimentDefinition experiment_definition_;

    std::unique_ptr<ExperimentManager> experiment_manager_;

    types::Uuid experiment_uuid_;
    types::Timeout duration_;
    NodeManager::ControlMessage deployment_message_;
    std::string experiment_name_;
    std::string library_path_;

    types::SocketAddress experiment_manager_publisher_endpoint_;
    types::SocketAddress experiment_manager_registration_endpoint_;
    types::SocketAddress experiment_logger_endpoint_;

    bool shutdown_ = false;

    // node_ip -> internal node map
    std::unordered_map<std::string, std::unique_ptr<EnvironmentManager::Node>> node_map_;

    // map of internal port_id -> external port unique label
    std::unordered_map<std::string, std::unique_ptr<ExternalPort>> external_port_map_;

    // map of internal port_id -> blackbox port
    std::unordered_map<std::string, std::unique_ptr<Port>> blackbox_port_map_;

    // external port unique label -> internal port id
    std::unordered_map<std::string, std::string> external_id_to_internal_id_map_;

    std::vector<types::Uuid> epm_list_;
    // Docker doesn't use nice types::Uuid compliant ids for their containers :(
    std::vector<std::string> docker_container_id_list_;

    ExperimentState state_ = ExperimentState::REGISTERED;

    // Set dirty flag when we've added a public port to the environment that this experiment cares
    // about. On next heartbeat we should send a control message with the endpoint of the public
    // port that we want to subscribe or publish to
    bool dirty_ = false;
    auto StartDockerProcess(types::Ipv4 node_ip) -> std::pair<types::Uuid, std::string>;

    struct EpmRegistration {
        types::Uuid request_uuid;
        types::Uuid epm_uuid;
    };

    EpmRegistrationReply HandleDockerEpmRegistration(const EpmRegistrationRequest& request);
    void HandleTriggerEvent(const TriggerEvent& event);

    std::mutex epm_registration_mutex_;
    std::queue<EpmRegistration> epm_registrations_;
    std::condition_variable epm_registration_semaphore_;
    static std::string BuildEpmRegistrationTopic(types::Uuid experiment_uuid);
    re::network::Replier<EpmRegistrationRequest, EpmRegistrationReply> epm_registration_replier_;
    re::network::Subscriber<TriggerEvent> trigger_handler_;
    types::Uuid WaitForEpmRegistrationMessage(const types::Uuid& request_uuid);

    std::vector<std::string>
    CreateDockerCommandString(types::Ipv4 node_ip, types::Uuid creation_request_uuid);

    JSON_DOCUMENT
    GetDockerJsonRequest(const types::Ipv4& node_ip, const types::Uuid& request_uuid);
    void KillDockerEpm(types::Uuid epm_uuid);
    re::EnvironmentManager::TriggerPrototype& GetTrigger(const std::string& basicString);

    types::Uuid StartEpmForContainer(Node& node, Container& container);

    auto Log(const std::vector<std::string>& messages) const -> void;
    auto LogError(const std::vector<std::string>& messages) const -> void;
    auto FormatLogMessage(const std::vector<std::string>& messages) const -> std::string;
    bool TriggerShouldFire(const std::string& trigger_id);

    // trigger_instance uuid -> last time the trigger fired.
    std::unordered_map<types::Uuid, std::chrono::system_clock::time_point> trigger_fire_times_;
};
}; // namespace re::EnvironmentManager

#endif // EXPERIMENT_H