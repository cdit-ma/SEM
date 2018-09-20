#ifndef NODEMANAGER_ENVIRONMENTREQUESTER_H
#define NODEMANAGER_ENVIRONMENTREQUESTER_H

#include <iostream>
#include <mutex>
#include <thread>
#include <queue>
#include <future>
#include "../protorequester/protorequester.hpp"


namespace NodeManager{
    class ControlMessage;
    class EnvironmentMessage;
};

namespace zmq{
    class context_t;
    class socket_t;
}

class EnvironmentRequester{
    public:
        enum class DeploymentType{
            RE_MASTER,
            RE_SLAVE,
            LOGAN
        };
        EnvironmentRequester(const std::string& manager_address,
                                const std::string& deployment_id,
                                DeploymentType deployment_type
                                );
        ~EnvironmentRequester();
        void Init(const std::string& manager_endpoint);
        void Start();
        void Terminate();

        NodeManager::ControlMessage AddDeployment(const NodeManager::ControlMessage& control_message);
        void RemoveDeployment();

        NodeManager::ControlMessage NodeQuery(const std::string& node_endpoint);
        
        NodeManager::EnvironmentMessage GetLoganInfo(const std::string& node_ip_address);
        void SetIPAddress(const std::string& ip_address);

        void AddUpdateCallback(std::function<void (NodeManager::EnvironmentMessage& environment_message)> callback_func);

    private:

        DeploymentType deployment_type_;

        //ZMQ endpoints
        std::string manager_address_;
        std::string manager_endpoint_;
        std::string manager_update_endpoint_;

        std::string ip_address_;

        bool environment_manager_not_found_ = false;
        
        std::future<void> heartbeat_future_;

        bool end_flag_ = false;

        std::unique_ptr<zmq::ProtoRequester> update_requester_;

        void HandleReply(NodeManager::EnvironmentMessage& message);

        std::string experiment_id_;

        //Callback
        std::function<void (NodeManager::EnvironmentMessage&)> update_callback_;
};

#endif //NODEMANAGER_ENVIRONMENTREQUESTER_H
