#ifndef ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#define ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#include <zmq.hpp>
#include <future>
#include <string>
#include <map>
#include <tuple>

#include "environment.h"
#include "environmentmessage/environmentmessage.pb.h"

class DeploymentHandler{
    public:
        DeploymentHandler(Environment* env, zmq::context_t* context, const std::string& ip_addr, 
                            std::promise<std::string>* port_promise, const std::string& info);

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

        typedef EnvironmentManager::EnvironmentMessage EnvMessage;

        EnvMessage HandleAddDeployment(uint64_t message_time, EnvMessage message);
        EnvMessage HandleRemoveDeployment(uint64_t message_time, EnvMessage message);

        EnvMessage HandleAddComponent(uint64_t message_time, EnvMessage message);
        EnvMessage HandleRemoveComponent(uint64_t message_time, EnvMessage message);

        EnvMessage HandleAddEndpoint(uint64_t message_time, EnvMessage message);
        EnvMessage HandleRemoveEndpoint(uint64_t message_time, EnvMessage message);

        EnvMessage HandleGetDeploymentInfo(uint64_t message_time, EnvMessage message);
        
        //Environment Helpers
        void RemoveDeployment();

        std::string TCPify(const std::string& ip, const std::string& port) const;
        std::string TCPify(const std::string& ip, int port) const;

        void ZMQSendReply(zmq::socket_t* socket, const std::string& message);
        std::pair<uint64_t, std::string> ZMQReceiveRequest(zmq::socket_t* socket);

        //Members
        std::string ip_addr_;
        zmq::context_t* context_;
        zmq::socket_t* handler_socket_;

        Environment* environment_;

        std::string deployment_id_;
        std::map<std::string, std::string> port_map_;
        long time_added_;

        std::thread* handler_thread_;

        std::promise<std::string>* port_promise_;
};

#endif //ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER