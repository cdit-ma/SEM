#ifndef ENVIRONMENT_MANAGER_ENVIRONMENT
#define ENVIRONMENT_MANAGER_ENVIRONMENT

#include <set>
#include <string>
#include <mutex>
#include <unordered_map>


class Environment{

    public:
        Environment();

        std::string AddDeployment(const std::string& deployment_id);
        void RemoveDeployment(const std::string& deployment_id);

        std::string AddComponent(const std::string& component_id);
        void RemoveComponent(const std::string& component_id);
    private:
        //Port range
        //TODO: Make this user configurable to avoid conflicts and allow multiple environments on the same network?
        static const int PORT_RANGE_MIN = 30000;
        static const int PORT_RANGE_MAX = 40000;


        std::mutex port_mutex_;
        std::set<int> available_ports_;
        std::unordered_map<std::string, std::string> component_port_map_;
        std::unordered_map<std::string, std::string> deployment_port_map_;

};

#endif //ENVIRONMENT_MANAGER_ENVIRONMENT
