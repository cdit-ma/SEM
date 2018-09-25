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

#include <core/modellogger.h>
#include "deploymentcontainer.h"

#include <proto/controlmessage/controlmessage.pb.h>
#include <zmq/protoreceiver/protoreceiver.h>
#include <zmq/protorequester/protorequester.hpp>
#include <proto/controlmessage/controlmessage.pb.h>
#include <util/execution.hpp>

class DeploymentManager{
    public:
        DeploymentManager(
            Execution& execution,
            const std::string& ip_address,
            const std::string& experiment_name,
            const std::string& master_publisher_endpoint,
            const std::string& master_registration_endpoint,
            const std::string& library_path,
            bool is_master_node);
        ~DeploymentManager();

        NodeManager::SlaveStartupResponse HandleSlaveStartup(const NodeManager::SlaveStartup startup);
        void HandleExperimentUpdate(const NodeManager::ControlMessage& control_message);
        void GotExperimentUpdate(const NodeManager::ControlMessage& control_message);
        void HandleNodeUpdate(const NodeManager::Node& node);

        void Terminate();
        void RequestDeployment();
    private:

        void ProcessControlQueue();
        void InteruptControlQueue();


        bool is_master_node_ = false;
        const std::string library_path_;
        const std::string ip_address_;
        const std::string experiment_name_;
        Execution& execution_;
        
        std::unique_ptr<zmq::ProtoReceiver> proto_receiever_;
        std::unique_ptr<zmq::ProtoRequester> proto_requester_;

        //Storage of DeploymentContainers
        std::mutex container_mutex_;
        std::unordered_map<std::string, std::shared_ptr<DeploymentContainer> > deployment_containers_;

        
        bool terminate_ = false;
        std::mutex queue_mutex_;
        std::queue<std::unique_ptr<NodeManager::ControlMessage> > control_message_queue_;
        std::condition_variable queue_condition_;
        
        std::future<void> control_queue_future_;
};

#endif //DEPLOYMENTMANAGER_H
