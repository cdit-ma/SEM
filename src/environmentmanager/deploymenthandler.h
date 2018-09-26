#ifndef ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#define ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#include <zmq.hpp>
#include <future>
#include <string>
#include <map>
#include <tuple>

#include "environment.h"
#include <zmq/protoreplier/protoreplier.hpp>
#include <proto/controlmessage/controlmessage.pb.h>

namespace NodeManager{
    class EnvironmentMessage;
};
class DeploymentHandler{
    public:
        
        DeploymentHandler(EnvironmentManager::Environment& env,
                        const std::string& ip_addr,
                        EnvironmentManager::Environment::DeploymentType deployment_type,
                        const std::string& deployment_ip_address,
                        std::promise<std::string> port_promise,
                        const std::string& experiment_id);
        
        ~DeploymentHandler();
    private:
        //Req/rep loops
        void HeartbeatLoop() noexcept;
        void RemoveDeployment();
        
        std::unique_ptr<NodeManager::EnvironmentMessage> HandleHeartbeat(const NodeManager::EnvironmentMessage& request_message);
        std::unique_ptr<NodeManager::NodeManagerDeregistrationReply> HandleNodeManagerDeregisteration(const NodeManager::NodeManagerDeregistrationRequest& request_message);
        std::unique_ptr<NodeManager::EnvironmentMessage> HandleGetExperimentInfo(const NodeManager::EnvironmentMessage& request_message);


        std::future<void> heartbeat_future_;
        const EnvironmentManager::Environment::DeploymentType deployment_type_;
        EnvironmentManager::Environment& environment_;
        
        const std::string environment_manager_ip_address_;
        const std::string deployment_ip_address_;
        const std::string experiment_id_;

        std::mutex replier_mutex_;
        std::unique_ptr<zmq::ProtoReplier> replier_;

        bool removed_flag_ = false;
};



#endif //ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER