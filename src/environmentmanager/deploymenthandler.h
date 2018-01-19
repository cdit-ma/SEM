#ifndef ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#define ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER
#include <zmq.hpp>
#include <future>
#include <string>

#include "environment.h"

class DeploymentHandler{
    public:
        DeploymentHandler(Environment* env, zmq::context_t* context, std::promise<std::string> port_promise, const std::string& info);

    private:
        //Heartbeat constatns
        static const int INITIAL_TIMEOUT = 4000;
        static const int HEARTBEAT_INTERVAL = 1000;
        static const int HEARTBEAT_LIVENESS = 3;
        static const int INITIAL_INTERVAL = 1000;
        static const int MAX_INTERVAL = 8000;

        void Init();
        void HeartbeatLoop();

        std::promise<std::string> port_promise_;

        std::string info_;

        zmq::context_t* context_;
        zmq::socket_t* handler_socket_;

        std::thread* handler_thread_;

        Environment* environment_;
};

#endif //ENVIRONMENT_MANAGER_DEPLOYMENTHANDLER