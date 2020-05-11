#ifndef DEPLOYMENTMANAGER_H
#define DEPLOYMENTMANAGER_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>

#include "deploymentcontainer.h"
#include "logger.h"
#include "slaveheartbeater.h"

#include "controlmessage.pb.h"
#include "ipv4.hpp"
#include "socketaddress.hpp"
#include "execution.hpp"
#include "protoreceiver.h"
#include "protorequester.hpp"

/// For the time being, this class is being used as an ExperimentProcess.
/// In the case where multiple experiment processes are needed for the same experiment on one node,
///  we currently start multiple EPMs.
class DeploymentManager {
public:
    // REVIEW (Mitch): Config struct, use endpoint class to be defined in util library, use
    //  std::filesystem path
    // REVIEW(Jackson): Can't actually use std::filesystem at this point in time
    DeploymentManager(re::types::Uuid experiment_uuid,
                      re::types::SocketAddress broker_address,
                      std::string experiment_name,
                      const re::types::Ipv4& ip_address,
                      std::string container_id,
                      const re::types::SocketAddress& master_publisher_endpoint,
                      const re::types::SocketAddress& master_registration_endpoint,
                      std::string library_path);
    ~DeploymentManager();

    // REVIEW (Mitch): Why are these public, internal use only
    void HandleExperimentUpdate(const NodeManager::ControlMessage& control_message);
    void GotExperimentUpdate(const NodeManager::ControlMessage& control_message);
    void
    HandleContainerUpdate(const std::string& host_name, const NodeManager::Container& container);

    void Terminate();
    void RequestDeployment();

private:
    re::types::Uuid experiment_uuid_;
    re::types::SocketAddress broker_address_;
    std::unique_ptr<NodeManager::SlaveId> GetSlaveID() const;
    void ProcessControlQueue();
    void InteruptControlQueue(); // REVIEW (Mitch): Typo in Interrupt

    const std::string library_path_;
    const re::types::Ipv4 ip_address_;
    // REVIEW (Mitch): "Container" is misleading. See earlier review comment.
    //  Will eventually rename to experiment_process_uuid and use re::types::Uuid type
    const std::string container_id_;
    const std::string experiment_name_;

    std::unique_ptr<zmq::ProtoReceiver> proto_receiever_; // REVIEW (Mitch): Typo in receiver
    std::unique_ptr<zmq::ProtoRequester> proto_requester_;
    std::unique_ptr<SlaveHeartbeater> heartbeater_;

    // Storage of DeploymentContainers
    std::mutex container_mutex_;
    // REVIEW (Mitch): what goes in this std::string?
    std::unordered_map<std::string, std::shared_ptr<DeploymentContainer>> deployment_containers_;

    bool has_been_activated_ = false;
    bool terminate_ = false;
    std::mutex queue_mutex_;
    std::queue<std::unique_ptr<NodeManager::ControlMessage>> control_message_queue_;
    std::condition_variable queue_condition_;

    std::future<void> control_queue_future_;
};

#endif // DEPLOYMENTMANAGER_H
