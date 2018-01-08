#ifndef ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER
#define ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER

#include <thread>
#include <unordered_map>
#include <zmq.hpp>
#include <mutex>

class DeploymentRegister{
    public:
        DeploymentRegister(const std::string& ip_addr, int registration_port, 
                            int heartbeat_start_port);

        void Start();
        void RegistrationLoop();
        void QueryLoop();
        void HeartbeatLoop(int port_no, const std::string& deployment_info);

        void AddDeployment(int port_no, const std::string& hb_endpoint);
        void RemoveDeployment(int port_no);
        void RemoveDeployment(const std::string& deployment_name);

        std::string GetDeploymentInfo() const;
        std::string GetDeploymentInfo(const std::string& name) const;

    private:
        std::string ip_addr_;
        zmq::context_t* context_;
        std::thread* registration_loop_;

        int registration_port_;
        int hb_start_port_ = 22338;
        int current_port_;

        std::unordered_map<int, std::string> deployments_;

        std::vector<std::string> manager_hb_endpoints_;
        std::vector<std::thread*> hb_threads_;

        std::mutex register_mutex_;
};


#endif //ENVIRONMENT_MANAGER_DEPLOYMENT_REGISTER