#ifndef ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#define ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#include <zmq.hpp>
#include <future>
#include <string>
#include <map>
#include <tuple>

#include "environment.h"
#include <proto/controlmessage/controlmessage.pb.h>

namespace NodeManager{
    class EnvironmentMessage;
};
class DeploymentHandler{
    public:
        
        DeploymentHandler(EnvironmentManager::Environment& env,
                        zmq::context_t& context,
                        const std::string& ip_addr,
                        EnvironmentManager::Environment::DeploymentType deployment_type,
                        const std::string& deployment_ip_address,
                        std::promise<std::string>* port_promise,
                        const std::string& experiment_id);

        void Terminate();

        void PrintError(const std::string& message);


    private:
        //Heartbeat constants (ms)
        static const int INITIAL_TIMEOUT = 4000;
        static const int HEARTBEAT_INTERVAL = 2000;
        static const int HEARTBEAT_LIVENESS = 3;
        static const int INITIAL_INTERVAL = 2000;
        static const int MAX_INTERVAL = 8000;

        //Req/rep loops
        void HeartbeatLoop() noexcept;

        //Reply Helpers
        std::string HandleRequest(std::pair<uint64_t, std::string> request);
        void HandleDirtyExperiment(NodeManager::EnvironmentMessage& message);
        void HandleLoganQuery(NodeManager::EnvironmentMessage& message);

        //Environment Helpers
        void RemoveDeployment(uint64_t message_time) noexcept;

        std::string TCPify(const std::string& ip, const std::string& port) const;
        std::string TCPify(const std::string& ip, int port) const;

        void ZMQSendReply(zmq::socket_t& socket, const std::string& message);
        std::pair<uint64_t, std::string> ZMQReceiveRequest(zmq::socket_t& socket);

        //Members
        std::string ip_addr_;
        zmq::context_t& context_;

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