#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <mutex>
#include <unordered_map>
#include <proto/controlmessage/controlmessage.pb.h>
#include "uniquequeue.hpp"

class Environment;

namespace EnvironmentManager{

struct LoganClient{
    std::string experiment_name;
    std::string id;
    std::string ip_address;
    std::string logging_port;
    double frequency;
    bool live_mode;
    std::vector<std::string> processes;
};

struct LoganServer{
    std::string experiment_name;
    std::string id;
    std::string ip_address;
    std::string db_file_name;
    std::vector<std::string> client_ids;
    std::vector<std::string> client_addresses;
};

class Node{
    public:
        Node(const std::string& ip_address, unique_queue<int> port_set){
            ip_ = ip_address;
            available_ports_ = port_set;
        }
        std::string GetPort(){
            std::unique_lock<std::mutex> lock(port_mutex_);
            if(available_ports_.empty()){
                return "";
            }
            auto port = available_ports_.front();
            available_ports_.pop();
            auto port_str =  std::to_string(port);
            std::cout << "Port Acquired: " << name_ << " : " << port_str << std::endl;
            return port_str;
        };

        void FreePort(const std::string& port){
            std::unique_lock<std::mutex> lock(port_mutex_);
            int port_number;
            try{
                port_number = std::stoi(port);
                if(available_ports_.push(port_number)){
                    std::cout << "Port Freed: " << name_ << " : " << port << std::endl;
                }
            }
            catch(const std::invalid_argument& ex){
                std::cerr << "Could not free port <\'" << port <<"\'>, port string could not be converted to int." << std::endl;
                std::cerr << ex.what() << std::endl;
            }
            catch(const std::out_of_range& ex){
                std::cerr << "Could not free port, port # out of range for int." << std::endl;
                std::cerr << ex.what() << std::endl;
            }
            catch(...){
                std::cerr << "Unknown exception thrown in Node::FreePort" << std::endl;
            }
        }
        std::string GetName(){
            return name_;
        };

        void SetName(const std::string& name){
            name_ = name;
        };
        std::string GetIp(){
            return ip_;
        };
        void IncrementComponentCount(){
            component_count_++;
        }
    private:
        std::string name_;
        std::string ip_;
        int component_count_ = 0;
        std::mutex port_mutex_;
        unique_queue<int> available_ports_;

        //eventport guid -> port number assigned
        std::unordered_map<std::string, std::string> used_ports_;
};

struct EventPort{
    std::string id;
    std::string guid;
    std::string type;
    std::string node_name;
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

class Experiment{
    public:
        Experiment(Environment& environment, std::string name);
        ~Experiment();
        
        void SetConfigureDone();
        bool ConfigureDone();

        std::string GetManagerPort() const;
        void SetManagerPort(const std::string& manager_port);

        void SetMasterIp(const std::string& ip);

        void AddNode(const NodeManager::Node& node);
        void ConfigureNode(NodeManager::Node& node);

        bool HasDeploymentOn(const std::string& node_name) const;

        NodeManager::EnvironmentMessage GetLoganDeploymentMessage(const std::string& ip_address);

        std::string GetMasterPublisherAddress();
        std::string GetMasterRegistrationAddress();

        std::string GetNodeModelLoggerPort(const std::string& ip) const;

        std::set<std::string> GetTopics() const;

        std::vector<std::string> GetPublisherAddress(const NodeManager::Port& port);
        std::string GetOrbEndpoint(const std::string& port_id);

        std::string GetTaoReplierServerAddress(const NodeManager::Port& port);
        std::string GetTaoServerName(const NodeManager::Port& port);

        bool IsDirty() const;

        void UpdatePort(const std::string& port_guid);

        void SetDeploymentMessage(const NodeManager::ControlMessage& control_message);
        void GetUpdate(NodeManager::ControlMessage& control_message);

    private:
    
        std::string GetNodeIpByName(const std::string& node_name);

        std::mutex mutex_;

        bool configure_done_;

        Environment& environment_;

        NodeManager::ControlMessage deployment_message_;
        std::string model_name_;
        std::string master_publisher_port_;
        std::string master_registration_port_;
        std::string master_ip_address_;
        std::string manager_port_;

        //node_id -> protobuf node
        std::unordered_map<std::string, std::unique_ptr<NodeManager::Node> > node_map_;

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

        //node_id -> orb port
        std::unordered_map<std::string, std::string> orb_port_map_;

        //list of topics used in this experiment
        std::set<std::string> topic_set_;

        //map of node id -> deployed component count
        std::unordered_map<std::string, int> deployment_map_;

        std::unordered_map<std::string, std::unique_ptr<LoganClient> > logan_client_map_;
        std::unordered_map<std::string, std::unique_ptr<LoganServer> > logan_server_map_;

        uint64_t time_added_;
        ExperimentState state_;

        //Set dirty flag when we've added a public port to the environment that this experiment cares about.
        //On next heartbeat we should send a control message with the endpoint of the public port that we want to subscribe or publish to
        bool dirty_flag_ = false;

        std::set<std::string> updated_port_ids_;
};
}; //namespace EnvironmentManager

#endif //EXPERIMENT_H