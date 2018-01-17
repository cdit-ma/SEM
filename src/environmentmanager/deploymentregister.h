#ifndef ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER
#define ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER

#include <thread>
#include <unordered_map>
#include <zmq.hpp>
#include <mutex>
#include <future>

class DeploymentRegister{
    public:
        DeploymentRegister(const std::string& ip_addr, const std::string& registration_port);

        void Start();
        void RegistrationLoop();
        void QueryLoop();
        void HeartbeatLoop(std::promise<std::string> assigned_port);

        void AddDeployment(const std::string& port_no, const std::string& hb_endpoint);
        void RemoveDeployment(const std::string& port_no);

        std::string HandleQuery(const std::string& query);
        std::string GetDeploymentInfo() const;
        std::string GetDeploymentInfo(const std::string& name) const;

    private:
        static const std::string SUCCESS;
        static const std::string ERROR;

        static const int INITIAL_TIMEOUT = 4000;
        static const int HEARTBEAT_INTERVAL = 1000;

        static const int HEARTBEAT_LIVENESS = 3;
        static const int INITIAL_INTERVAL = 1000;
        static const int MAX_INTERVAL = 8000;

        void SendTwoPartReply(zmq::socket_t* socket, const std::string& part_one, const std::string& part_two);

        std::string TCPify(const std::string& ip_address, const std::string& port) const;
        std::string TCPify(const std::string& ip_address, int port) const;

        zmq::context_t* context_;

        std::string ip_addr_;
        std::string registration_port_;

        std::thread* registration_loop_;
        std::vector<std::thread*> hb_threads_;

        std::mutex register_mutex_;
        std::unordered_map<std::string, std::string> deployment_map_;

};


#endif //ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER