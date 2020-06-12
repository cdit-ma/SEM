#ifndef ENVIRONMENT_MANAGER_ENVIRONMENT
#define ENVIRONMENT_MANAGER_ENVIRONMENT

#include <set>
#include <string>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <proto/controlmessage/controlmessage.pb.h>
#include "uniquequeue.hpp"
#include "experiment.h"
#include "porttracker.h"
#include <zmq/protowriter/protowriter.h>

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
            LOGAN_SERVER
        };

        Environment(const std::string& ip_address,
                const std::string& qpid_broker_address,
                const std::string& tao_naming_service_address,
                int port_range_min = 30000,
                int port_range_max = 50000);
        ~Environment();

        Experiment& GetExperiment(const std::string& experiment_name);
        void PopulateExperiment(const NodeManager::Experiment &message);


        std::unique_ptr<NodeManager::RegisterExperimentReply> GetExperimentDeploymentInfo(const std::string& experiment_name);

        std::string GetDeploymentHandlerPort(const std::string& experiment_name, DeploymentType deployment_type);
        std::string GetUpdatePublisherPort() const;



        void ShutdownExperiment(const std::string& experiment_name);
        std::vector<std::string> ShutdownExperimentRegex(const std::string& experiment_name_regex);

        void RemoveExperiment(const std::string& experiment_name);
        
        std::unique_ptr<NodeManager::EnvironmentMessage> GetProto(const std::string& experiment_name, const bool full_update);

        bool ExperimentIsDirty(const std::string& experiment_name);
        bool GotExperiment(const std::string& experiment_name) const;
        
        bool IsExperimentRegistered(const std::string& experiment_name);
        bool IsExperimentActive(const std::string& experiment_name);
        bool IsExperimentConfigured(const std::string& experiment_name);


        std::vector<std::string> GetExperimentNames() const;
        std::vector<std::unique_ptr<NodeManager::ExternalPort> > GetExternalPorts() const;
        

        bool NodeDeployedTo(const std::string& experiment_name, const std::string& ip_address);
        
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
    private:
        void ShutdownExperimentInternal(const std::string& experiment_name);
        std::vector<std::string> GetMatchingExperiments(const std::string& experiment_name_regex);

        std::string GetExperimentHandlerPort(const std::string& experiment_name);

        void RemoveExperimentInternal(const std::string& experiment_name);
        void DistributeContainerToImplicitNodeContainers(const std::string& experiment_id, const NodeManager::Container& container);
        void DeployContainer(const std::string& experiment_id, const NodeManager::Container& container);
        
        void AddNodeToExperiment(const std::string& experiment_name, const NodeManager::Node& node);
        void AddNodeToEnvironment(const NodeManager::Node& node);
        ExternalPort& GetExternalPort(const std::string& external_port_label);
        Experiment& GetExperimentInternal(const std::string& experiment_name);
        void AddNodes(const std::string& experiment_id, const NodeManager::Cluster& cluster);

        //Port range
        int port_range_min_;
        int port_range_max_;

        int manager_port_range_min_;
        int manager_port_range_max_;

        std::string ip_address_;

        std::string qpid_broker_address_;
        std::string tao_naming_service_address_;

        void RegisterExperiment(const std::string& experiment_name);

        std::mutex configure_experiment_mutex_;

        mutable std::mutex experiment_mutex_;
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

        std::string update_publisher_port_;
        std::unique_ptr<::zmq::ProtoWriter> update_publisher_;

        std::mutex port_mutex_;
        
        
        //initially allocated unique_queue of port nums from port_range_min_ to port_range_max_ so we can copy into each node struct
        unique_queue<int> available_ports_;

        //ports available on the environment manager, uses same port range as nodes.
        unique_queue<int> available_node_manager_ports_;
};
};

#endif //ENVIRONMENT_MANAGER_ENVIRONMENT
