#ifndef EXPERIMENT_PROCESS_MANAGER_H
#define EXPERIMENT_PROCESS_MANAGER_H

#include <functional>
#include <string>
#include <thread>
#include <unordered_map>

#include "deploymentmanager.h"
#include "nodemanagerconfig.h"
#include "epmcontrol.pb.h"
#include "epmregistration.pb.h"
#include "epmstdout.pb.h"

#include "publisher.hpp"
#include "replier.hpp"
#include "ipv4.hpp"
#include "socketaddress.hpp"
#include "uuid.h"

namespace re::NodeManager {
class ExperimentProcessManager {
public:
    struct ExperimentProcessManagerConfig {
        types::Uuid epm_uuid;
        types::Uuid experiment_uuid;
        types::Uuid creation_request_uuid;
        types::Uuid registration_entity_uuid;
        types::Ipv4 ip_address;
        types::SocketAddress qpid_broker_endpoint;
        std::string lib_root_dir{};
        std::string re_bin_path{};
    };
    explicit ExperimentProcessManager(ExperimentProcessManagerConfig config);
    auto Start() -> void;

    static auto HandleArguments(int argc, char** argv) -> ExperimentProcessManagerConfig;

private:
    using RegistrationRequest = network::protocol::epmregistration::Request;
    using RegistrationReply = network::protocol::epmregistration::Reply;
    using ControlRequest = network::protocol::epmcontrol::Request;
    using ControlReply = network::protocol::epmcontrol::Reply;

    using StdOutMessage = network::protocol::epmstdout::Message;

    ExperimentProcessManagerConfig epm_config_;

    std::vector<std::unique_ptr<DeploymentManager>> deployment_managers_;

    /// Replier listening on topic "<experiment_id>_experiment_management"
    network::Replier<ControlRequest, ControlReply> epm_control_replier_;

    auto BuildEpmControlTopic() -> std::string;
    auto BuildEpmRegistrationTopic() -> std::string;
    auto RegisterWithNodeManager(const types::Uuid& request_id) -> void;
    auto HandleEpmControl(const ControlRequest& request) -> ControlReply;

    /// Stdout capture tools
    auto SendStdOut() -> void;
    std::stringstream buffer_;
    // This is our one and only handle to the original std::out. After we've redirected std::out to
    //  our buffer_, we need to keep a handle s.t. we can eventually reset std::out to point to this
    std::streambuf* original_std_out_stream_pointer_;
    std::future<void> std_out_sender_future_;
    std::atomic_bool stop_std_out_capture_ = false;
    std::string BuildEpmStdOutTopic();
    void StopStdOutCapture();
    void PushStdOutMessage(network::Publisher<StdOutMessage>& publisher);
    static auto GetErrorReply(const std::string& error_message) -> ControlReply;

    std::mutex running_mutex_;
    std::condition_variable running_cv_;
    bool finished_ = false;
};
} // namespace re::NodeManager

#endif // EXPERIMENT_PROCESS_MANAGER_H
