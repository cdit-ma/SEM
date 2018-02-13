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

        std::string AddDeployment(const std::string& deployment_id, const std::string& proto_info, uint64_t time_called);
        void RemoveDeployment(const std::string& deployment_id, uint64_t time_called);

        std::string AddMasterPort(const std::string& deployment_id, uint64_t time_called);
        void RemoveMasterPort(const std::string& deployment_id, uint64_t time_called);

        std::string AddModelLoggerPort(const std::string& deployment_id, uint64_t time_called);
        void RemoveModelLoggerPort(const std::string& deployment_id, uint64_t time_called);

        void DeploymentLive(const std::string& deployment_id, uint64_t time_called);
        void DeploymentTimeout(const std::string& deployment_id, uint64_t time_called);

        std::string AddComponent(const std::string& deployment_id, const std::string& component_id, 
                                    const std::string& proto_info, uint64_t time_called);
        void RemoveComponent(const std::string& component_id, uint64_t time_called);

        std::string AddEndpoint();
        void RemoveEndpoint();

        std::string GetMasterPort(const std::string& deployment_id);
        std::string GetModelLoggerPort(const std::string& deployment_id);

        uint64_t GetClock();
        uint64_t SetClock(uint64_t clock);
        uint64_t Tick();
    private:
        uint64_t clock_;
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
            std::string ip_addr;
            std::string master_port;
            std::string model_logger_port;
            std::string environment_manager_port;
            std::vector<std::string> component_ids;
            std::vector<std::string> public_component_ids;
            uint64_t time_added;
            //TODO: Add endpoint information s.t. MEDEA can query
        };

        std::mutex port_mutex_;
        std::set<int> available_ports_;
        std::unordered_map<std::string, std::string> component_port_map_;
        std::unordered_map<std::string, std::string> deployment_port_map_;
        std::unordered_map<std::string, Deployment*> deployment_info_map_;
};

#endif //ENVIRONMENT_MANAGER_ENVIRONMENT
