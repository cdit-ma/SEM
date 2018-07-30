#ifndef ENVIRONMENT_MANAGER_ENVIRONMENT
#define ENVIRONMENT_MANAGER_ENVIRONMENT

#include <set>
#include <string>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <re_common/proto/controlmessage/controlmessage.pb.h>
#include "uniquequeue.hpp"
#include "experiment.h"
#include "porttracker.h"

namespace EnvironmentManager{

class Environment{
    private:
        struct ExternalPort{
            std::string external_label;
            std::set<std::string> producer_experiments;
            std::set<std::string> consumer_experiments;
        };

    public: 
        enum class DeploymentType{
            EXECUTION_MASTER,
            EXECTUION_SLAVE,
            LOGAN_CLIENT,
            LOGAN_SERVER
        };

        Environment(const std::string& address, const std::string& qpid_broker_address, const std::string& tao_naming_service_address, int portrange_min = 30000, int portrange_max = 50000);

        void PopulateExperiment(NodeManager::ControlMessage& message);

        std::string AddDeployment(const std::string& experiment_name, const std::string& ip_address, DeploymentType deployment_type);

        void RemoveExperiment(const std::string& experiment_name, uint64_t time);
        void RemoveLoganClientServer(const std::string& experiment_name, const std::string& ip_address);

        NodeManager::EnvironmentMessage* GetLoganDeploymentMessage(const std::string& experiment_name, const std::string& ip_address);

        NodeManager::ControlMessage* GetProto(const std::string& experiment_name);

        bool ExperimentIsDirty(const std::string& experiment_name);
        bool GotExperiment(const std::string& experiment_name);
        NodeManager::ControlMessage* GetExperimentUpdate(const std::string& experiment_name);
        NodeManager::ControlMessage* GetLoganUpdate(const std::string& experiment_name);
        

        bool IsExperimentConfigured(const std::string& experiment_name);
        bool NodeDeployedTo(const std::string& experiment_name, const std::string& ip_address);
        
        void SetExperimentMasterIp(const std::string& experiment_name, const std::string& ip_address);

        std::string GetMasterPublisherAddress(const std::string& experiment_name);
        std::string GetMasterRegistrationAddress(const std::string& experiment_name);

        std::string GetTopic(const std::string& experiment_name, const std::string& port_id);

        std::vector<std::string> CheckTopic(const std::string& experiment_name, const std::string& topic);

        std::string GetPort(const std::string& ip_address);
        void FreePort(const std::string& ip_address, const std::string& port_number);

        std::string GetManagerPort();
        void FreeManagerPort(const std::string& port);

        std::vector< std::reference_wrapper<Port> > GetExternalProducerPorts(const std::string& external_port_label);

        void AddExternalConsumerPort(const std::string& experiment_name, const std::string& external_port_label);
        void AddExternalProducerPort(const std::string& experiment_name, const std::string& external_port_label);

        void RemoveExternalConsumerPort(const std::string& experiment_name, const std::string& external_port_label);
        void RemoveExternalProducerPort(const std::string& experiment_name, const std::string& external_port_label);

        std::string GetAmqpBrokerAddress();
        std::string GetTaoNamingServiceAddress();

        static const NodeManager::Attribute& GetAttributeByName(const google::protobuf::RepeatedPtrField<NodeManager::Attribute>& attribute_list, const std::string& attribute_name);

        uint64_t GetClock();
        uint64_t SetClock(uint64_t clock);
        uint64_t Tick();
    private:
        void FinishConfigure(const std::string& experiment_name);
        static void DeclusterExperiment(NodeManager::ControlMessage& message);
        static void DeclusterNode(NodeManager::Node& message);
        
        
        void AddExternalPorts(const std::string& experiment_name, const NodeManager::ControlMessage& control_message);
        void AddNodeToExperiment(const std::string& experiment_name, const NodeManager::Node& node);
        void AddNodeToEnvironment(const NodeManager::Node& node);
        ExternalPort& GetExternalPort(const std::string& external_port_label);
        Experiment& GetExperiment(const std::string experiment_name);
        void RecursiveAddNode(const std::string& experiment_id, const NodeManager::Node& node);

        std::mutex clock_mutex_;
        uint64_t clock_;

        //Port range
        int PORT_RANGE_MIN;
        int PORT_RANGE_MAX;

        int MANAGER_PORT_RANGE_MIN;
        int MANAGER_PORT_RANGE_MAX;

        std::string address_;

        std::string qpid_broker_address_;
        std::string tao_naming_service_address_;

        //Returns management port for logan client to communicate with environment_manager
        std::string AddLoganClientServer();

        //Returns management port for re_node_manager(master) to communicate with environment_manager
        std::string RegisterExperiment(const std::string& experiment_name);

        std::mutex configure_experiment_mutex_;

        std::mutex experiment_mutex_;
        //experiment_name -> experiment data structure
        std::unordered_map<std::string, std::unique_ptr<EnvironmentManager::Experiment> > experiment_map_;

        std::mutex node_mutex_;
        //node_name -> node data structure
        std::unordered_map<std::string, std::unique_ptr<EnvironmentManager::PortTracker> > node_map_;

        //node_name -> node_ip map
        std::unordered_map<std::string, std::string> node_ip_map_;

        //node_ip -> node_name map
        std::unordered_map<std::string, std::string> node_name_map_;

        //event port guid -> event port data structure
        //event port guid takes form "experiment_id.{component_assembly_label}*n.component_instance_label.event_port_label"
        std::unordered_map<std::string, std::unique_ptr<ExternalPort> > external_eventport_map_;

        std::mutex port_mutex_;
        
        
        //initially allocated unique_queue of port nums from PORT_RANGE_MIN to PORT_RANGE_MAX so we can copy into each node struct
        unique_queue<int> available_ports_;

        //ports available on the environment manager, uses same port range as nodes.
        unique_queue<int> available_node_manager_ports_;
};
};

#endif //ENVIRONMENT_MANAGER_ENVIRONMENT
