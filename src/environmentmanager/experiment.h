#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <mutex>
#include <unordered_map>
#include <proto/controlmessage/controlmessage.pb.h>
#include "ports/port.h"
#include "uniquequeue.hpp"


namespace EnvironmentManager{
class Environment;
class Node;
class Port;
class Logger;
class Container;

class Experiment{
    public:

        struct ExternalPort{
            enum class Kind{
                PubSub,
                ReqRep
            };

            std::string external_label;
            std::string internal_id;

            std::string type;

            Port::Middleware middleware;
            Kind kind;

            std::set<std::string> producer_ids;
            std::set<std::string> consumer_ids;
        };

        enum class ExperimentState{
            REGISTERED,
            CONFIGURED,
            ACTIVE
        };
        Experiment(Environment& environment, std::string name);
        ~Experiment();
        
        ExperimentState GetState() const;

        void SetConfigured();
        void SetActive();

        bool IsRegistered() const;
        bool IsConfigured() const;
        bool IsActive() const;

        void CheckValidity() const;

        const std::string& GetName() const;

        void ConfigureMaster();
        std::string GetMessage() const;

        std::string GetManagerPort() const;
        void SetManagerPort(const std::string& manager_port);

        void AddExternalPorts(const NodeManager::Experiment& message);
        void AddNode(const NodeManager::Node& node);
        void AddLoggingClientToNodes(const NodeManager::Logger& logging_client);

        Node& GetNode(const std::string& ip_address) const;

        void Shutdown();

        bool HasDeploymentOn(const std::string& node_name) const;

        Environment& GetEnvironment() const;

        std::unique_ptr<NodeManager::RegisterExperimentReply> GetDeploymentInfo();
        std::vector<std::unique_ptr<NodeManager::Logger> > GetAllocatedLoganServers(const std::string& ip_address);
        std::unique_ptr<NodeManager::EnvironmentMessage> GetProto(const bool full_update);

        std::string GetMasterPublisherAddress();
        std::string GetMasterRegistrationAddress();
        const std::string& GetMasterIp() const;

        std::string GetOrbEndpoint(const std::string& port_id);

        std::string GetPublicEventPortName(const std::string& public_port_local_id);

        Node& GetLeastDeployedToNode(bool non_empty = false);
        Port& GetPort(const std::string& id);

        bool IsDirty() const;
        void SetDirty();

        void UpdatePort(const std::string& external_port_label);

        void SetDeploymentMessage(const NodeManager::ControlMessage& control_message);

        std::vector< std::reference_wrapper<Port> > GetExternalProducerPorts(const std::string& external_port_label);
        std::vector< std::reference_wrapper<Logger> > GetLoggerClients(const std::string& logger_id);


        std::string GetExternalPortLabel(const std::string& internal_port_id);
        std::string GetExternalPortInternalId(const std::string& external_port_label);

        void AddExternalConsumerPort(const std::string& external_port_internal_id, const std::string& internal_port_id);
        void AddExternalProducerPort(const std::string& external_port_internal_id, const std::string& internal_port_id);
        void RemoveExternalConsumerPort(const std::string& external_port_internal_id, const std::string& internal_port_id);
        void RemoveExternalProducerPort(const std::string& external_port_internal_id, const std::string& internal_port_id);

        std::vector< std::reference_wrapper<ExternalPort> > GetExternalPorts() const;
    private:
        void SetMasterIp(const std::string& ip);
        ExternalPort& GetExternalPort(const std::string& external_port_internal_id);

        mutable std::mutex mutex_;

        Environment& environment_;

        NodeManager::ControlMessage deployment_message_;
        std::string model_name_;
        std::string master_publisher_port_;
        std::string master_registration_port_;
        std::string master_ip_address_;
        std::string manager_port_;


        bool shutdown_ = false;

        //node_ip -> internal node map
        std::unordered_map<std::string, std::unique_ptr<EnvironmentManager::Node> > node_map_;

        //map of internal port_id -> external port unique label
        std::unordered_map<std::string, std::unique_ptr<ExternalPort> > external_port_map_;

        //map of internal port_id -> blackbox port
        std::unordered_map<std::string, std::unique_ptr<Port> > blackbox_port_map_;

        //external port unique label -> internal port id
        std::unordered_map<std::string, std::string> external_id_to_internal_id_map_;

        ExperimentState state_ = ExperimentState::REGISTERED;

        //Set dirty flag when we've added a public port to the environment that this experiment cares about.
        //On next heartbeat we should send a control message with the endpoint of the public port that we want to subscribe or publish to
        bool dirty_ = false;
};
}; //namespace EnvironmentManager

#endif //EXPERIMENT_H