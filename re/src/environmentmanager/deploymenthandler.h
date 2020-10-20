#ifndef ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#define ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#include <future>
#include <map>
#include <string>
#include <tuple>
#include <zmq.hpp>

#include "environment/environment.h"
#include "protoreplier.hpp"
#include "controlmessage.pb.h"

namespace NodeManager {
class EnvironmentMessage;
};
namespace re::EnvironmentManager {
// REVIEW (Mitch): This class name is super misleading. We only actually start a deployment handler
//  for logan servers.

// TODO: Review this documentation.
/**
 * Effectively equivalent to ExperimentManager but for logan servers
 */
class DeploymentHandler {
public:
    DeploymentHandler(EnvironmentManager::Environment& env,
                      const types::Ipv4& environment_manager_ip_address,
                      EnvironmentManager::Environment::DeploymentType deployment_type,
                      const types::Ipv4& deployment_ip_address,
                      std::promise<uint16_t> port_promise,
                      types::Uuid experiment_uuid);

    bool IsRemovable() const;
    ~DeploymentHandler();

private:
    // Req/rep loops
    void HeartbeatLoop() noexcept;
    void RemoveDeployment();

    std::unique_ptr<NodeManager::EnvironmentMessage>
    HandleHeartbeat(const NodeManager::EnvironmentMessage& request_message);

    std::unique_ptr<NodeManager::NodeManagerDeregistrationReply> HandleNodeManagerDeregisteration(
        const NodeManager::NodeManagerDeregistrationRequest& request_message);

    std::unique_ptr<NodeManager::EnvironmentMessage>
    HandleGetExperimentInfo(const NodeManager::EnvironmentMessage& request_message);

    std::future<void> heartbeat_future_;
    EnvironmentManager::Environment& environment_;
    const types::Ipv4 environment_manager_ip_address_;
    const EnvironmentManager::Environment::DeploymentType deployment_type_;
    const types::Ipv4 deployment_ip_address_;
    const types::Uuid experiment_uuid_;

    std::mutex replier_mutex_;
    std::unique_ptr<zmq::ProtoReplier> replier_;

    mutable std::mutex remove_mutex_;

    bool removed_flag_ = false;
};
} // namespace re::EnvironmentManager

#endif // ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
