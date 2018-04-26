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
        void StoreControlMessage(const NodeManager::ControlMessage& control_message);
        
        void DeclusterExperiment(NodeManager::ControlMessage& message);
        void DeclusterNode(NodeManager::Node& message);
        void AddNodeToExperiment(const std::string& model_name, const NodeManager::Node& node);
        void AddNodeToEnvironment(const NodeManager::Node& node);
        void ConfigureNode(const std::string& model_name, NodeManager::Node& node);

        bool ExperimentIsDirty(const std::string& model_name);
        void GetExperimentUpdate(const std::string& model_name, NodeManager::ControlMessage& control_message);

        bool ModelNameExists(const std::string& model_name) const;
        bool NodeDeployedTo(const std::string& model_name, const std::string& ip_address) const;
        std::string GetMasterPublisherPort(const std::string& model_name, const std::string& master_ip_address);
        std::string GetNodeManagementPort(const std::string& model_name, const std::string& ip_address);
        std::string GetNodeModelLoggerPort(const std::string& model_name, const std::string& ip_address);
        std::string GetNodeHardwareLoggerPort(const std::string& model_name, const std::string& ip_address);

        std::vector<std::string> GetPublisherAddress(const std::string& model_name, const NodeManager::EventPort& port);
        std::string GetTopic(const std::string& model_name, const std::string& port_id);

        std::vector<std::string> CheckTopic(const std::string& model_name, const std::string& topic);

        std::string GetPort(const std::string& node_ip);
        void FreePort(const std::string& node_ip, const std::string& port_number);
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

        int MANAGER_PORT_RANGE_MIN;
        int MANAGER_PORT_RANGE_MAX;

        std::string GetManagerPort();
        void FreeManagerPort(const std::string& port);

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
                    auto port_str =  std::to_string(port);
                    return port_str;
                };

                void FreePort(const std::string& port){
                    std::unique_lock<std::mutex> lock(port_mutex);
                    int port_number;
                    try{
                        port_number = std::stoi(port);
                        available_ports.insert(port_number);
                    }
                    catch(const std::invalid_argument& ex){
                        std::cerr << "Could not free port, port string could not be converted to int." << std::endl;
                        std::cerr << ex.what() << std::endl;
                    }
                    catch(const std::out_of_range& ex){
                        std::cerr << "Could not free port, port # out of range for int." << std::endl;
                        std::cerr << ex.what() << std::endl;
                    }
                }
                std::string name;
                std::string ip;
                int component_count = 0;
            private:
                std::mutex port_mutex;
                std::set<int> available_ports;

                //eventport guid -> port number assigned
                std::map<std::string, std::string> used_ports;

        };

        struct EventPort{
            std::string id;
            std::string guid;
            std::string type;
            std::string node_id;
            std::string port_number;
            std::string topic;

            //list of publisher ids
            std::vector<std::string> connected_ports;

            std::string endpoint;

        };

        enum class ExperimentState{
            ACTIVE,
            TIMEOUT,
            SHUTDOWN
        };

        struct Experiment{
            Experiment(std::string name){model_name_ = name;};

            std::mutex mutex_;

            NodeManager::ControlMessage deployment_message_;
            std::string model_name_;
            std::string master_port_;
            std::string master_ip_address_;
            std::string manager_port_;

            //node_id -> protobuf node
            std::unordered_map<std::string, NodeManager::Node*> node_map_;

            //node_id -> node_ip_addr
            std::unordered_map<std::string, std::string> node_address_map_;

            //node_ip_address -> node_id
            std::unordered_map<std::string, std::string> node_id_map_;

            //event port id -> eventport
            std::unordered_map<std::string, EventPort> port_map_;

            //subscriber_id -> publisher_id
            std::unordered_map<std::string, std::vector<std::string> > connection_map_;

            //node_id -> management_port
            std::unordered_map<std::string, std::string> management_port_map_;

            //node_id -> model_logger_port
            std::unordered_map<std::string, std::string> modellogger_port_map_;

            //node_id -> hardware_logger_port
            std::unordered_map<std::string, std::string> hardwarelogger_port_map_;

            //list of topics used in this experiment
            std::set<std::string> topic_set_;

            //map of node id -> deployed component count
            std::unordered_map<std::string, int> deployment_map_;

            uint64_t time_added;
            ExperimentState state;

            //Set dirty flag when we've added a public port to the environment that this experiment cares about.
            //On next heartbeat we should send a control message with the endpoint of the public port that we want to subscribe or publish to
            bool dirty_flag = false;

            std::set<std::string> updated_port_ids_;

        };

        //model_name -> experiment data structure
        std::unordered_map<std::string, Experiment*> experiment_map_;

        //node_ip -> node data structure
        std::unordered_map<std::string, Node*> node_map_;

        //event port guid -> event port data structure
        //event port guid takes form "experiment_id.{component_assembly_label}*n.component_instance_label.event_port_label"
        std::unordered_map<std::string, EventPort> public_event_port_map_;

        //event port guid -> set of experiment ids
        //keeps track of experiments waiting for port of this guid to become live.
        std::unordered_map<std::string, std::set<std::string> > pending_port_map_;

        std::mutex port_mutex_;
        //initially allocated set of port nums from PORT_RANGE_MIN to PORT_RANGE_MAX so we can copy into each node struct
        std::set<int> available_ports_;

        //ports available on the environment manager, uses same port range as nodes.
        std::set<int> available_node_manager_ports_;
};

#endif //ENVIRONMENT_MANAGER_ENVIRONMENT
