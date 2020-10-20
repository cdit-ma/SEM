#ifndef RE_NODEMANAGER_H
#define RE_NODEMANAGER_H

#include "experimentprocessmanager.h"
#include "nodemanagerconfig.h"
#include <boost/process/child.hpp>
#include <future>
#include <memory>
#include "epmregistration.pb.h"
#include "nodemanagercontrol.pb.h"
#include "nodemanagerregistration.pb.h"
#include "replier.hpp"
#include "requester.hpp"
#include "controlmessage.pb.h"
#include "execution.hpp"
namespace re::NodeManager {

class NodeManager {
public:
    explicit NodeManager(NodeConfig config);
    auto Run() -> void;
    auto Stop() -> void;
    auto Deregister() -> void;

private:
    using NodeManagerRegistration =
        re::network::protocol::nodemanagerregistration::Request;
    using NodeManagerRegistrationReply =
        re::network::protocol::nodemanagerregistration::Reply;

    using EnvironmentManagerRequestType = network::protocol::nodemanagercontrol::Request;
    using EnvironmentManagerReplyType =
        network::protocol::nodemanagercontrol::Reply;

    using EpmRegistrationRequest = network::protocol::epmregistration::Request;
    using EpmRegistrationReply = network::protocol::epmregistration::Reply;

    static auto RegisterNodeManager(const NodeConfig& config) -> void;
    auto HandleEpmManagementRequest(const EnvironmentManagerRequestType& request)
        -> EnvironmentManagerReplyType;
    auto NewEpm(const sem::types::Uuid& uuid, const std::string& experiment_name) -> sem::types::Uuid;
    auto HandleEpmRegistration(const EpmRegistrationRequest& request) -> EpmRegistrationReply;
    static auto BuildEpmStartCommand(const NodeConfig& node_config,
                                     sem::types::Uuid experiment_uuid,
                                     sem::types::Uuid creation_request_id) -> std::string;

    static constexpr std::string_view node_manager_registration_topic_ = "nodemanager_"
                                                                         "registration_topic";

    std::mutex epm_registration_mutex_;
    std::condition_variable epm_registration_semaphore_;
    const NodeConfig node_config_;

    re::network::Replier<EnvironmentManagerRequestType, EnvironmentManagerReplyType>
        environment_manager_replier_;

    re::network::Replier<EpmRegistrationRequest, EpmRegistrationReply> epm_registration_replier_;

    struct EpmRegistration {
        sem::types::Uuid request_uuid;
        sem::types::Uuid epm_uuid;
    };

    std::queue<EpmRegistration> epm_registrations_;
    std::unordered_map<sem::types::Uuid, std::unique_ptr<boost::process::child>> epm_process_handles_;

    std::future<void> heartbeat_loop_handle_;
    std::mutex heartbeat_mutex_;
    std::condition_variable heartbeat_cv_;
    std::atomic<bool> end_heartbeat_{false};

    std::future<void> run_handle_;

    static auto BuildControlTopicName(const sem::types::Uuid& uuid) -> std::string;
    static auto BuildEpmRegistrationTopicName(const sem::types::Uuid& uuid) -> std::string;
    static auto FindEpmExecutable(const std::string& re_bin_path) -> std::string;

    auto WaitForEpmRegistrationMessage(const sem::types::Uuid& request_uuid) -> sem::types::Uuid;
    void StopEpm(sem::types::Uuid uuid);
    auto HeartbeatLoop() -> void;
    void Heartbeat();

    static auto GetRegistrationRequester(const NodeConfig& config)
        -> network::Requester<NodeManagerRegistration, NodeManagerRegistrationReply>;
};

} // namespace re::NodeManager

#endif // RE_NODEMANAGER_H
