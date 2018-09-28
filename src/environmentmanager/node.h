#ifndef ENVIRONMENT_MANAGER_NODE_H
#define ENVIRONMENT_MANAGER_NODE_H

#include <memory>
#include <mutex>
#include <unordered_map>

#include "uniquequeue.hpp"
#include <proto/controlmessage/controlmessage.pb.h>

namespace EnvironmentManager{

class Environment;
class Component;
class Logger;
class Port;
class Experiment;
class Attribute;

class Node{
    public:
        Node(Environment& environment, Experiment& experiment, const NodeManager::Node& node);
        ~Node();
        Node(const Node& parent, const std::string& id, const std::string& name, const std::string& ip_address);

        //Info getters
        std::string GetId() const;
        std::string GetName() const;
        std::string GetIp() const;
        int GetDeployedComponentCount() const;
        int GetDeployedCount() const;
        
        bool IsNodeManagerMaster() const;
        void SetNodeManagerMaster();

        std::unique_ptr<NodeManager::HardwareId> GetHardwareId() const;


        //Fully qualified endpoint getters
        std::string GetManagementEndpoint() const;
        std::string GetOrbEndpoint() const;

        //Port number only getters
        std::string GetManagementPort() const;
        bool HasOrbPort() const;
        std::string GetOrbPort() const;
        std::string AssignOrbPort();
        std::vector<std::string> GetAllPublisherPorts() const;
        std::vector<std::string> GetAllLoggingPorts() const;

        //Port number setters
        void SetManagementPort(const std::string& management_port);
        void SetModelLoggerPort(const std::string& management_port);
        void SetOrbPort(const std::string& orb_port);

        //Node, component, attribute and logger adders
        void AddComponent(const NodeManager::Component& component);
        void AddAttribute(const NodeManager::Attribute& attribute);
        void AddLogger(const NodeManager::Logger& logger);
        void AddModelLogger();

        int GetLoganServerCount() const;
        int GetLoganClientCount() const;

        bool HasLogger(const std::string& logger_id);
        Logger& GetLogger(const std::string& logger_id);

        //Update requirement management
        void SetDirty();
        bool IsDirty();

        Experiment& GetExperiment();

        //Deep getters
        bool HasNode(const std::string& node_id) const;
        Node& GetNode(const std::string& node_id) const;
        bool HasComponent(const std::string& component_id) const;
        Component& GetComponent(const std::string& component_id) const;
        bool HasPort(const std::string& port_id) const;
        Port& GetPort(const std::string& port_id) const;

        bool DeployedTo() const;

        //protobuf getters
        std::unique_ptr<NodeManager::Node> GetProto(const bool full_update);
        
        std::vector<std::unique_ptr<NodeManager::Logger> > GetAllocatedLoganServers() const;


    private:
        Environment& environment_;
        Experiment& experiment_;
        std::string id_;
        std::string name_;
        std::string ip_;

        std::string management_port_;
        std::string model_logger_port_;
        std::string orb_port_;

        //node id -> node struct
        std::unordered_map<std::string, std::unique_ptr<Node> > nodes_;

        //Component id -> Component
        std::unordered_map<std::string, std::unique_ptr<Component> > components_;

        //logger id -> Logger
        std::unordered_map<std::string, std::unique_ptr<Logger> > loggers_;

        //attribute id -> attribute
        std::unordered_map<std::string, std::unique_ptr<Attribute> > attributes_;



        bool dirty_;
        bool is_node_manager_master_ = false;
};
};

#endif //ENVIRONMENT_MANAGER_NODE_H