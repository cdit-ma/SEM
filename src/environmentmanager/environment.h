#ifndef ENVIRONMENT_MANAGER_ENVIRONMENT
#define ENVIRONMENT_MANAGER_ENVIRONMENT

#include <set>
#include <string>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "controlmessage.pb.h"

class Environment{

    public:
        Environment(int portrange_min = 30000, int portrange_max = 50000);

        void AddExperiment(const NodeManager::ControlMessage& message);
        void DeclusterExperiment(NodeManager::ControlMessage& message);
        void DeclusterNode(NodeManager::Node& message);
        void AddNodeToExperiment(const std::string& model_name, const NodeManager::Node& node);
        void AddNodeToEnvironment(const NodeManager::Node& node);

        std::vector<std::string> GetPublisherAddress(const std::string& model_name, const std::string& port_id);
        std::string GetTopic(const std::string& model_name, const std::string& port_id);
        
        std::string GetPort(const std::string& node_ip);

        NodeManager::Node GetDeploymentLocation(const std::string& model_name, const std::string& port_id);

        std::string AddDeployment(const std::string& deployment_id, const std::string& proto_info, uint64_t time_called);
        void RemoveDeployment(const std::string& deployment_id, uint64_t time_called);

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
        int PORT_RANGE_MIN;
        int PORT_RANGE_MAX;

        struct Node{
            Node(const std::string& name, std::set<int> port_set){
                this->name = name;
                available_ports = port_set;
            }
            std::string name;
            std::string ip;
            std::set<int> available_ports;

            //type
        };

        struct EventPort{
            std::string id;
            std::string node_id;
            std::string port_number;
            std::string guid;
            std::string topic;

            std::vector<std::string> connected_ports;
        };

        struct Experiment{
            Experiment(std::string name){model_name_ = name;};
            NodeManager::ControlMessage deployment_message_;
            std::string model_name_;
            std::string master_port_;
            std::string manager_port_;
            std::unordered_map<std::string, NodeManager::Node*> node_map_;
            std::unordered_map<std::string, std::string> node_address_map_;

            std::unordered_map<std::string, EventPort> port_map_;

            std::unordered_map<std::string, std::vector<std::string> > connection_map_;

        };

        std::unordered_map<std::string, Experiment*> experiment_map_;
        std::unordered_map<std::string, Node*> node_map_;

        enum class DeploymentState{
            ACTIVE,
            TIMEOUT,
            SHUTDOWN
        };
        enum class Middleware{
            NO_MIDDLEWARE = 0,
            ZMQ = 1,
            RTI_DDS = 2,
            OSPL_DDS = 3,
            QPID = 4,
            CORBA = 5
        };

        enum class EndpointType{
            NO_TYPE = 0,
            MANAGEMENT = 1,
            DEPLOYMENT_MASTER = 2,
            MODEL_LOGGER = 3,
            PUBLIC = 4,
            PRIVATE = 5
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

        struct Component{
            std::string id;
            std::string deployment_id;
            
        };

        struct Endpoint{
            std::string id;
            EndpointType type;
            Middleware middleware;
        };


        std::mutex port_mutex_;
        std::set<int> available_ports_;
        std::unordered_map<std::string, std::string> component_port_map_;
        std::unordered_map<std::string, std::string> deployment_port_map_;
        std::unordered_map<std::string, Deployment*> deployment_info_map_;
};

#endif //ENVIRONMENT_MANAGER_ENVIRONMENT
