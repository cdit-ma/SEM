#ifndef ENVIRONMENT_MANAGER_ENVIRONMENT
#define ENVIRONMENT_MANAGER_ENVIRONMENT

#include <set>
#include <string>
#include <mutex>
#include <unordered_map>
#include <future>
#include <zmq.hpp>

class Environment{

    public:
        Environment();

        std::string AddDeployment(std::promise<std::string> port_promise, const std::string& component_id);
        void RemoveDeployment(const std::string& deployment_id);
        //TODO: Store other deployment details, endpoints etc.
        std::string GetDeployment(const std::string& deployment_id);

    private:
        //Port range
        //TODO: Make this user configurable to avoid conflicts and allow multiple environments on the same network?
        static const int PORT_RANGE_MIN = 30000;
        static const int PORT_RANGE_MAX = 40000;

        std::string AddPort(const std::string& component_id);
        void RemovePort(const std::string& component_id);

        std::mutex port_mutex_;
        std::set<int> available_ports_;
        std::unordered_map<std::string, DeploymentHandler*> deployment_map_;
        std::unordered_map<std::string, std::string> component_port_map_;

        zmq::context_t* context_;


};

#endif //ENVIRONMENT_MANAGER_ENVIRONMENT