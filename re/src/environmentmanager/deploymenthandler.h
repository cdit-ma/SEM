#ifndef ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#define ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#include <zmq.hpp>
#include <future>
#include <string>
#include <map>
#include <tuple>

#include "environment.h"
#include "protoreplier.hpp"
#include "controlmessage.pb.h"

namespace NodeManager{
    class EnvironmentMessage;
};
class DeploymentHandler{
    public:
        
        DeploymentHandler(EnvironmentManager::Environment& env,
                        const std::string& environment_manager_ip_address,
                        EnvironmentManager::Environment::DeploymentType deployment_type,
                        const std::string& deployment_ip_address,
                        std::promise<std::string> port_promise,
                        const std::string& experiment_id);


        bool IsRemovable() const;
        ~DeploymentHandler();
    private:
        //Req/rep loops
        void HeartbeatLoop() noexcept;
        void RemoveDeployment();
        
        std::unique_ptr<NodeManager::EnvironmentMessage> HandleHeartbeat(const NodeManager::EnvironmentMessage& request_message);
        std::unique_ptr<NodeManager::NodeManagerDeregistrationReply> HandleNodeManagerDeregisteration(const NodeManager::NodeManagerDeregistrationRequest& request_message);
        std::unique_ptr<NodeManager::EnvironmentMessage> HandleGetExperimentInfo(const NodeManager::EnvironmentMessage& request_message);


        std::future<void> heartbeat_future_;
        EnvironmentManager::Environment& environment_;
        const std::string environment_manager_ip_address_;
        const EnvironmentManager::Environment::DeploymentType deployment_type_;
        const std::string deployment_ip_address_;
        const std::string experiment_id_;

        std::mutex replier_mutex_;
        std::unique_ptr<zmq::ProtoReplier> replier_;


        mutable std::mutex remove_mutex_;

        bool removed_flag_ = false;
};



#endif //ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER