#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <mutex>
#include <unordered_map>
#include <proto/controlmessage/controlmessage.pb.h>
#include "uniquequeue.hpp"


namespace EnvironmentManager{
class Environment;
class Node;
class Port;
struct ExternalPort{
    std::string id;
    std::string external_label;
    std::set<std::string> connected_ports;
    std::string endpoint;

    bool is_blackbox = false;
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

        void AddExternalPorts(const NodeManager::ControlMessage& message);
        void AddNode(const NodeManager::Node& node);
        void ConfigureNodes();

        NodeManager::ControlMessage* GetProto();

        bool HasDeploymentOn(const std::string& node_name) const;

        NodeManager::EnvironmentMessage* GetLoganDeploymentMessage(const std::string& ip_address);

        std::string GetMasterPublisherAddress();
        std::string GetMasterRegistrationAddress();

        std::string GetNodeModelLoggerPort(const std::string& ip) const;

        std::set<std::string> GetTopics() const;

        std::vector<std::string> GetPublisherAddress(const NodeManager::Port& port);
        std::string GetOrbEndpoint(const std::string& port_id);

        std::string GetTaoReplierServerAddress(const NodeManager::Port& port);
        std::string GetTaoServerName(const NodeManager::Port& port);

        std::string GetPublicEventPortName(const std::string& public_port_local_id);

        Port& GetPort(const std::string& id);


        bool IsDirty() const;
        void SetDirty();


        void UpdatePort(const std::string& external_port_label);

        void SetDeploymentMessage(const NodeManager::ControlMessage& control_message);
        NodeManager::ControlMessage* GetUpdate();

        void AddLoganClientEndpoint(const std::string& client_id, const std::string& endoint);
        void RemoveLoganClientEndpoint(const std::string& client_id);
        const std::unordered_map<std::string, std::string>& GetLoganClientEndpointMap() const;

        void AddModelLoggerEndpoint(const std::string& client_id, const std::string& endoint);
        void RemoveModelLoggerEndpoint(const std::string& client_id);
        const std::unordered_map<std::string, std::string>& GetModelLoggerEndpointMap() const;

        void AddZmqEndpoint(const std::string& port_id, const std::string& endpoint);
        void RemoveZmqEndpoint(const std::string& port_id);
        const std::unordered_map<std::string, std::string>& GetZmqEndpointMap() const;

        void AddTaoEndpoint(const std::string& port_id, const std::string& endpoint);
        void RemoveTaoEndpoint(const std::string& port_id);
        const std::unordered_map<std::string, std::string>& GetTaoEndpointMap() const;

        void AddConnection(const std::string& connected_id, const std::string& port_id);
        void AddTopic(const std::string& topic);

        void AddExternalEndpoint(const std::string& external_port_internal_id, const std::string& endpoint);



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

        //node_ip -> internal node map
        std::unordered_map<std::string, std::unique_ptr<EnvironmentManager::Node> > node_map_;

        //node_id -> node_ip_addr
        std::unordered_map<std::string, std::string> node_address_map_;

        //node_ip_address -> node_id
        std::unordered_map<std::string, std::string> node_id_map_;

        //list of topics used in this experiment
        std::set<std::string> topic_set_;

        //map of internal port_id -> external port unique label
        std::unordered_map<std::string, std::unique_ptr<ExternalPort> > external_port_map_;

        //external port_id -> internal port id
        std::unordered_map<std::string, std::string> external_id_to_internal_id_map_;

        uint64_t time_added_;
        ExperimentState state_;

        //Set dirty flag when we've added a public port to the environment that this experiment cares about.
        //On next heartbeat we should send a control message with the endpoint of the public port that we want to subscribe or publish to
        bool dirty_flag_ = false;

        std::set<std::string> updated_port_ids_;

        //node id -> model logger fully qualified address "tcp://xxx.xxx.xxx.xxx:pppp"
        std::unordered_map<std::string, std::string> modellogger_endpoint_map_;

        //logan client id -> logan client fully qualified address "tcp://xxx.xxx.xxx.xxx:pppp"
        std::unordered_map<std::string, std::string> logan_client_endpoint_map_;

        //port id -> configured zmq endpoing "tcp://xxx.xxx.xxx.xxx:pppp"
        std::unordered_map<std::string, std::string> zmq_endpoint_map_;

        //Port id -> Configured tao endpoint "corbaloc:iiop:xxx.xxx.xxx.xxx:pppp/server_name;
        std::unordered_map<std::string, std::string> tao_endpoint_map_;

};
}; //namespace EnvironmentManager

#endif //EXPERIMENT_H