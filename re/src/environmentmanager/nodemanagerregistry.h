#ifndef RE_NODEMANAGERREGISTRY_H
#define RE_NODEMANAGERREGISTRY_H

#include <network/protocols/epmcontrol/epmcontrol.pb.h>
#include <network/protocols/nodemanagercontrol/nodemanagercontrol.pb.h>
#include <network/protocols/nodemanagerregistration/nodemanagerregistration.pb.h>
#include <network/replier.hpp>
#include <types/uuid.h>

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
    types::SocketAddress broker_address_;

    re::network::Replier<NodeManagerRegistrationRequestType, NodeManagerRegistrationReplyType>
        replier_;

    static constexpr std::string_view nodemanager_registration_topic_ = "nodemanager_registration_"
                                                                        "topic";

    auto HandleRequest(const NodeManagerRegistrationRequestType& request)
        -> NodeManagerRegistrationReplyType;

    struct NodeManager{
        types::Uuid uuid;
        std::string hostname;
        std::chrono::steady_clock::time_point last_heartbeat;
    };
    std::mutex node_managers_lock_;
    std::unordered_map<types::Uuid, NodeManager> node_managers_;

    // EPM uuid -> the EPM's host node manager's uuid
    // This may need to change once we can launch docker based EPMs??
    std::unordered_map<types::Uuid, types::Uuid> epm_location_map_;

    static auto BuildNodeManagerControlTopicName(types::Uuid node_uuid) -> std::string;
    static auto BuildEpmControlTopicName(types::Uuid epm_uuid) -> std::string;
    auto HeartbeatLoop() -> void;

public:
    explicit NodeManagerRegistry(types::SocketAddress broker_address);
    ~NodeManagerRegistry();
    auto stop() -> void;
    auto GetNodeManagerUuid(const std::string& hostname) -> types::Uuid;
    auto NewEpm(types::Uuid node_manager_uuid,
                types::Uuid experiment_uuid,
                const std::string& experiment_name) -> types::Uuid;
    auto StopEpm(types::Uuid epm_uuid) -> void;
    auto KillBareMetalEpm(types::Uuid epm_uuid) -> void;
    auto StartExperimentProcess(types::Uuid epm_uuid,
                                const std::string& container_id,
                                types::SocketAddress manager_pubisher_endpoint,
                                types::SocketAddress manager_registration_endpoint,
                                const std::string& library_path) -> void;
    auto UpdateLastHeartbeatTime(types::Uuid uuid) -> void;
};
} // namespace re::EnvironmentManager

#endif // RE_NODEMANAGERREGISTRY_H
