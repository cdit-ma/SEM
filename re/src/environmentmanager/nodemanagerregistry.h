#ifndef RE_NODEMANAGERREGISTRY_H
#define RE_NODEMANAGERREGISTRY_H

#include "epmcontrol.pb.h"
#include "nodemanagercontrol.pb.h"
#include "nodemanagerregistration.pb.h"
#include "replier.hpp"
#include "uuid.h"

namespace re::EnvironmentManager {

/** Node manager registration point. Should be one instance of this per environment manager.
 *
 *  This should be used as a single point of communication between the environment manager and any
 *   node manager.
 */
class NodeManagerRegistry {
    using NodeManagerRegistrationReplyType =
        re::network::protocol::nodemanagerregistration::Reply;
    using NodeManagerRegistrationRequestType =
        re::network::protocol::nodemanagerregistration::Request;

    using NodeManagerControlRequestType =
        re::network::protocol::nodemanagercontrol::Request;
    using NodeManagerControlReplyType =
        re::network::protocol::nodemanagercontrol::Reply;

    using EpmControl = re::network::protocol::epmcontrol::Request;
    using EpmControlReply = re::network::protocol::epmcontrol::Reply;

    std::future<void> heartbeat_loop_handle_;
    std::mutex heartbeat_mutex_;
    std::condition_variable heartbeat_cv_;
    std::atomic<bool> end_heartbeat_{false};
    sem::types::SocketAddress broker_address_;

    re::network::Replier<NodeManagerRegistrationRequestType, NodeManagerRegistrationReplyType>
        replier_;

    static constexpr std::string_view nodemanager_registration_topic_ = "nodemanager_registration_"
                                                                        "topic";

    auto HandleRequest(const NodeManagerRegistrationRequestType& request)
        -> NodeManagerRegistrationReplyType;

    struct NodeManager{
        sem::types::Uuid uuid;
        std::string hostname;
        std::chrono::steady_clock::time_point last_heartbeat;
    };
    std::mutex node_managers_lock_;
    std::unordered_map<sem::types::Uuid, NodeManager> node_managers_;

    // EPM uuid -> the EPM's host node manager's uuid
    // This may need to change once we can launch docker based EPMs??
    std::unordered_map<sem::types::Uuid, sem::types::Uuid> epm_location_map_;

    static auto BuildNodeManagerControlTopicName(sem::types::Uuid node_uuid) -> std::string;
    static auto BuildEpmControlTopicName(sem::types::Uuid epm_uuid) -> std::string;
    auto HeartbeatLoop() -> void;

public:
    explicit NodeManagerRegistry(sem::types::SocketAddress broker_address);
    ~NodeManagerRegistry();
    auto stop() -> void;
    auto GetNodeManagerUuid(const std::string& hostname) -> sem::types::Uuid;
    auto NewEpm(sem::types::Uuid node_manager_uuid,
                sem::types::Uuid experiment_uuid,
                const std::string& experiment_name) -> sem::types::Uuid;
    auto StopEpm(sem::types::Uuid epm_uuid) -> void;
    auto KillBareMetalEpm(sem::types::Uuid epm_uuid) -> void;
    auto StartExperimentProcess(sem::types::Uuid epm_uuid,
                                const std::string& container_id,
                                sem::types::SocketAddress manager_pubisher_endpoint,
                                sem::types::SocketAddress manager_registration_endpoint,
                                const std::string& library_path) -> void;
    auto UpdateLastHeartbeatTime(sem::types::Uuid uuid) -> void;
};
} // namespace re::EnvironmentManager

#endif // RE_NODEMANAGERREGISTRY_H
