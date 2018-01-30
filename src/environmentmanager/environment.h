#ifndef ENVIRONMENT_MANAGER_ENVIRONMENT
#define ENVIRONMENT_MANAGER_ENVIRONMENT

#include <set>
#include <string>
#include <mutex>
#include <unordered_map>
#include <vector>

class Environment{

    public:
        Environment();

        std::string AddDeployment(const std::string& deployment_id, const std::string& proto_info, long time_added);
        void RemoveDeployment(const std::string& deployment_id, long time_added);

        std::string AddComponent(const std::string& deployment_id, const std::string& component_id, 
                                    const std::string& proto_info);
        void RemoveComponent(const std::string& component_id);

        long GetClock();
        long SetClock(long clock);
        long Tick();
    private:
        long clock_;
        std::mutex clock_mutex_;

        //Port range
        //TODO: Make this user configurable to avoid conflicts and allow multiple environments on the same network?
        static const int PORT_RANGE_MIN = 30000;
        static const int PORT_RANGE_MAX = 40000;

        enum class DeploymentState{
            ACTIVE,
            TIMEOUT,
            SHUTDOWN
        };

        struct Deployment{
            std::string id;
            DeploymentState state;
            std::vector<std::string> component_ids;
            long time_added;
            //TODO: Add endpoint information s.t. MEDEA can query
        };

        std::mutex port_mutex_;
        std::set<int> available_ports_;
        std::unordered_map<std::string, std::string> component_port_map_;
        std::unordered_map<std::string, std::string> deployment_port_map_;
        std::unordered_map<std::string, Deployment*> deployment_info_map_;
};

#endif //ENVIRONMENT_MANAGER_ENVIRONMENT
