#ifndef ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER
#define ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER

#include <thread>
#include <unordered_map>
#include <set>
#include <zmq.hpp>
#include <mutex>
#include <future>
#include "environment.h"
#include "deploymenthandler.h"

class DeploymentRegister{
    public:
        DeploymentRegister(const std::string& ip_addr, const std::string& registration_port);

        void Start();

    private:
        //Constants
        static const std::string SUCCESS;
        static const std::string ERROR;

        //Threads and thread wrappers
        void RegistrationLoop();
        void HandleDeployment(std::promise<std::string> assigned_port, const std::string& request_info);

        //Helpers
        std::string TCPify(const std::string& ip_address, const std::string& port) const;
        std::string TCPify(const std::string& ip_address, int port) const;

        void ZMQSendReply(zmq::socket_t* socket, const std::string& part_two);
        std::pair<uint64_t, std::string> ZMQReceiveRequest(zmq::socket_t* socket);

        //Members
        zmq::context_t* context_;

        Environment* environment_;

        std::string ip_addr_;
        std::string registration_port_;

        std::thread* registration_loop_;
        std::vector<DeploymentHandler*> deployments_;
        std::vector<std::thread*> handler_threads_;

};

#endif //ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER
