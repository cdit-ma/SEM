#ifndef NODEMANAGER_ENVIRONMENTREQUEST_H
#define NODEMANAGER_ENVIRONMENTREQUEST_H

#include <mutex>
#include <memory>
#include <functional>
#include <zmq/protorequester/protorequester.hpp>
#include "heartbeater.h"

namespace NodeManager{
    class EnvironmentMessage;
};

namespace EnvironmentRequest{
    class HeartbeatRequester{
        public:
            HeartbeatRequester(const std::string& heartbeat_endpoint,
                                          std::function<void (NodeManager::EnvironmentMessage& environment_message)> configure_function);
            ~HeartbeatRequester();
            void Terminate();
            void RemoveDeployment();

            void SetTimeoutCallback(std::function<void (void)> func);

            std::unique_ptr<NodeManager::EnvironmentMessage> GetExperimentInfo();
        private:
            std::mutex heartbeater_mutex_;
            std::function<void (NodeManager::EnvironmentMessage&)> update_callback_;
            std::unique_ptr<zmq::ProtoRequester> requester_;
            std::unique_ptr<Heartbeater> heartbeater_;
    };

    std::unique_ptr<NodeManager::NodeManagerRegistrationReply> TryRegisterNodeManager(const std::string& environment_manager_endpoint, const std::string& experiment_name, const std::string& node_ip_address);
    std::unique_ptr<NodeManager::LoganRegistrationReply> TryRegisterLoganServer(const std::string& environment_manager_endpoint, const std::string& experiment_name, const std::string& node_ip_address);

    std::unique_ptr<NodeManager::NodeManagerRegistrationReply> TryRegisterNodeManager(const std::string& environment_manager_endpoint, const std::string& experiment_name, const std::string& node_ip_address);
    
};

#endif //NODEMANAGER_ENVIRONMENTREQUEST_H
