#ifndef ENVIRONMENT_MANAGER_ENVIRONMENT
#define ENVIRONMENT_MANAGER_ENVIRONMENT

#include <set>
#include <string>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <iostream>

#include "controlmessage.pb.h"

class Environment{

    public:
        Environment(int portrange_min = 30000, int portrange_max = 50000);

        std::string AddExperiment(const std::string& model_name);
        void RemoveExperiment(const std::string& model_name, uint64_t time);
        
        void DeclusterExperiment(NodeManager::ControlMessage& message);
        void DeclusterNode(NodeManager::Node& message);
        void AddNodeToExperiment(const std::string& model_name, const NodeManager::Node& node);
        void AddNodeToEnvironment(const NodeManager::Node& node);
        void ConfigureNode(NodeManager::Node& node);

        std::vector<std::string> GetPublisherAddress(const std::string& model_name, const std::string& port_id);
        std::string GetTopic(const std::string& model_name, const std::string& port_id);
        
        std::string GetPort(const std::string& node_ip);
        void ExperimentLive(const std::string& deployment_id, uint64_t time_called);
        void ExperimentTimeout(const std::string& deployment_id, uint64_t time_called);

        NodeManager::Node GetDeploymentLocation(const std::string& model_name, const std::string& port_id);

        uint64_t GetClock();
        uint64_t SetClock(uint64_t clock);
        uint64_t Tick();
    private:
        uint64_t clock_;
        std::mutex clock_mutex_;

        //Port range
        int PORT_RANGE_MIN;
        int PORT_RANGE_MAX;

        class Node{
            public:
                Node(const std::string& name, std::set<int> port_set){
                    this->name = name;
                    available_ports = port_set;
                }
                std::string GetPort(){
                    std::unique_lock<std::mutex> lock(port_mutex);
                    if(available_ports.empty()){
                        return "";
                    }
                    auto it = available_ports.begin();
                    auto port = *it;
                    available_ports.erase(it);
                    std::cout << name << std::endl;
                    std::cout << available_ports.size() << std::endl;
                    return std::to_string(port);
                };

                void FreePort(const std::string& port){
                    std::unique_lock<std::mutex> lock(port_mutex);
                    int port_number = std::stoi(port);
                    available_ports.insert(port_number);
                }
                std::string name;
                std::string ip;
                int component_count = 0;
            private:
                std::mutex port_mutex;
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

        enum class ExperimentState{
            ACTIVE,
            TIMEOUT,
            SHUTDOWN
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

            uint64_t time_added;
            ExperimentState state;

        };

        std::unordered_map<std::string, Experiment*> experiment_map_;
        std::unordered_map<std::string, Node*> node_map_;


        std::mutex port_mutex_;
        std::set<int> available_ports_;
        std::set<int> available_node_manager_ports_;

};

#endif //ENVIRONMENT_MANAGER_ENVIRONMENT
