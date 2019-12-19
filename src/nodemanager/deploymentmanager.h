#ifndef DEPLOYMENTMANAGER_H
#define DEPLOYMENTMANAGER_H

#include <functional>
#include <string>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include <unordered_map>
#include <memory>
#include <future>

#include <core/logger.h>
#include "deploymentcontainer.h"
#include "slaveheartbeater.h"

#include <proto/controlmessage/controlmessage.pb.h>
#include <zmq/protoreceiver/protoreceiver.h>
#include <zmq/protorequester/protorequester.hpp>
#include <proto/controlmessage/controlmessage.pb.h>
#include <util/execution.hpp>

// REVIEW (Mitch): DOCUMENTATION: One of these started per node used in the experiment.
//  A "node" in this context is either an instance of re_node_manager running on a node or
//  an instance of re_node_manager running in a docker container
class DeploymentManager{
    public:
    // REVIEW (Mitch): Config struct, use endpoint class to be defined in util library, use
    //  std::filesystem path
    // REVIEW(Jackson): Can't actually use std::filesystem at this point in time
    DeploymentManager(Execution& execution,
            const std::string& experiment_name,
            const std::string& ip_address,
            const std::string& container_id,
            const std::string& master_publisher_endpoint,
            const std::string& master_registration_endpoint,
            const std::string& library_path,
            bool is_master_node);
        ~DeploymentManager();

        // REVIEW (Mitch): Why are these public, internal use only
        NodeManager::SlaveStartupResponse HandleSlaveStartup(const NodeManager::SlaveStartup startup);
        void HandleExperimentUpdate(const NodeManager::ControlMessage& control_message);
        void GotExperimentUpdate(const NodeManager::ControlMessage& control_message);
        void HandleContainerUpdate(const std::string& host_name, const NodeManager::Container& container);

        void Terminate();
        void RequestDeployment();
    private:
        std::unique_ptr<NodeManager::SlaveId> GetSlaveID() const;
        void ProcessControlQueue();
        void InteruptControlQueue(); // REVIEW (Mitch): Typo in Interrupt

        bool is_master_node_ = false;
        const std::string library_path_;
        const std::string ip_address_;
        // REVIEW (Mitch): "Container" is misleading. See earlier review comment.
        const std::string container_id_;
        const std::string experiment_name_;
        Execution& execution_;

        std::unique_ptr<zmq::ProtoReceiver> proto_receiever_; // REVIEW (Mitch): Typo in receiver
        std::unique_ptr<zmq::ProtoRequester> proto_requester_;
        std::unique_ptr<SlaveHeartbeater> heartbeater_;

        //Storage of DeploymentContainers
        std::mutex container_mutex_;
        // REVIEW (Mitch): what goes in this std::string?
        std::unordered_map<std::string, std::shared_ptr<DeploymentContainer> > deployment_containers_;

        
        bool terminate_ = false;
        std::mutex queue_mutex_;
        std::queue<std::unique_ptr<NodeManager::ControlMessage> > control_message_queue_;
        std::condition_variable queue_condition_;
        
        std::future<void> control_queue_future_;
};

#endif //DEPLOYMENTMANAGER_H
