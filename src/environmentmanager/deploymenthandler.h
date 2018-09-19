#ifndef ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#define ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#include <zmq.hpp>
#include <future>
#include <string>
#include <map>
#include <tuple>

#include "environment.h"
#include <re_common/proto/controlmessage/controlmessage.pb.h>
#include <re_common/zmq/protoreplier/protoreplier.hpp>

namespace NodeManager{
    class EnvironmentMessage;
};
class DeploymentHandler{
    public:
        
        DeploymentHandler(EnvironmentManager::Environment& env,
                        const std::string& ip_addr,
                        EnvironmentManager::Environment::DeploymentType deployment_type,
                        const std::string& deployment_ip_address,
                        std::promise<std::string>* port_promise,
                        const std::string& experiment_id);

        void Terminate();
        void PrintError(const std::string& message);

    private:
        //Req/rep loops
        void HeartbeatLoop() noexcept;

        //Reply Helpers
        std::unique_ptr<NodeManager::EnvironmentMessage> HandleRequest(const NodeManager::EnvironmentMessage& request_message);
        void HandleDirtyExperiment(NodeManager::EnvironmentMessage& message);
        void HandleLoganQuery(NodeManager::EnvironmentMessage& message);

        //Environment Helpers
        void RemoveDeployment() noexcept;

        std::string TCPify(const std::string& ip, const std::string& port) const;
        std::string TCPify(const std::string& ip, int port) const;

        //Members
        std::string ip_addr_;
        std::unique_ptr<zmq::ProtoReplier> replier_;

        EnvironmentManager::Environment& environment_;

        EnvironmentManager::Environment::DeploymentType deployment_type_;
        std::string deployment_ip_address_;
        std::string experiment_id_;
        std::map<std::string, std::string> port_map_;
        long time_added_;

        std::mutex logan_ip_mutex_;
        std::set<std::string> registered_logan_ip_addresses;

        std::unique_ptr<std::thread> handler_thread_;

        std::promise<std::string>* port_promise_;

        bool removed_flag_ = false;
        bool terminate_ = false;
};



#endif //ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER