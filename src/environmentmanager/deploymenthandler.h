#ifndef ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#define ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#include <zmq.hpp>
#include <future>
#include <string>
#include <map>

#include "environment.h"

class DeploymentHandler{
    public:
        DeploymentHandler(Environment* env, zmq::context_t* context, const std::string& ip_addr, 
                            std::promise<std::string>* port_promise, const std::string& info);

    private:
        //Heartbeat constants (ms)
        static const int INITIAL_TIMEOUT = 4000;
        static const int HEARTBEAT_INTERVAL = 1000;
        static const int HEARTBEAT_LIVENESS = 3;
        static const int INITIAL_INTERVAL = 1000;
        static const int MAX_INTERVAL = 8000;

        //Req/rep loops
        void Init();
        void HeartbeatLoop();

        //Helpers
        void RemoveDeployment();

        std::string TCPify(const std::string& ip, const std::string& port) const;
        std::string TCPify(const std::string& ip, int port) const;

        void SendTwoPartReply(zmq::socket_t* socket, const std::string& part_one, const std::string& part_two);
        std::pair<std::string, std::string> ReceiveTwoPartRequest(zmq::socket_t* socket);

        //Members
        std::string ip_addr_;
        zmq::context_t* context_;
        zmq::socket_t* handler_socket_;

        Environment* environment_;

        std::string deployment_id_;
        std::map<std::string, std::string> port_map_;

        std::thread* handler_thread_;

        std::promise<std::string>* port_promise_;
};

#endif //ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER