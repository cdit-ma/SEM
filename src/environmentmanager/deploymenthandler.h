#ifndef ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#define ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#include <zmq.hpp>
#include <future>
#include <string>
#include <map>
#include <tuple>

#include "environment.h"
#include <src/re_common/proto/controlmessage/controlmessage.pb.h>

namespace NodeManager{
    class EnvironmentMessage;
};
class DeploymentHandler{
    public:
        
        DeploymentHandler(Environment* env,
                        zmq::context_t* context,
                        const std::string& ip_addr,
                        Environment::DeploymentType deployment_type,
                        const std::string& deployment_ip_address,
                        std::promise<std::string>* port_promise,
                        const std::string& experiment_id);


    private:
        //Heartbeat constants (ms)
        static const int INITIAL_TIMEOUT = 4000;
        static const int HEARTBEAT_INTERVAL = 2000;
        static const int HEARTBEAT_LIVENESS = 3;
        static const int INITIAL_INTERVAL = 2000;
        static const int MAX_INTERVAL = 8000;

        //Req/rep loops
        void Init();
        void HeartbeatLoop();

        //Reply Helpers
        void HandleRequest(std::pair<uint64_t, std::string> request);
        void HandleDirtyExperiment(NodeManager::EnvironmentMessage& message);
        void HandleLoganQuery(NodeManager::EnvironmentMessage& message);

        //Environment Helpers
        void RemoveDeployment(uint64_t message_time);

        std::string TCPify(const std::string& ip, const std::string& port) const;
        std::string TCPify(const std::string& ip, int port) const;

        void ZMQSendReply(zmq::socket_t* socket, const std::string& message);
        std::pair<uint64_t, std::string> ZMQReceiveRequest(zmq::socket_t* socket);

        //Members
        std::string ip_addr_;
        zmq::context_t* context_;
        zmq::socket_t* handler_socket_;

        Environment* environment_;

        Environment::DeploymentType deployment_type_;
        std::string deployment_ip_address_;
        std::string experiment_id_;
        std::map<std::string, std::string> port_map_;
        long time_added_;

        std::thread* handler_thread_;

        std::promise<std::string>* port_promise_;

        bool removed_flag_ = false;
};

#endif //ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER