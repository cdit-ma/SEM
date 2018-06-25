#ifndef ENVIRONMENT_MANAGER_NODE_H
#define ENVIRONMENT_MANAGER_NODE_H

#include <memory>
#include <mutex>
#include <unordered_map>

#include "uniquequeue.hpp"
#include <proto/controlmessage/controlmessage.pb.h>


namespace EnvironmentManager{
class Component;
class LoganServer;
class LoganClient;
class Node{
    public:
        Node(const std::string& id, const std::string& name, const std::string& ip_address);
        Node(const Node& parent, const std::string& id, const std::string& name, const std::string& ip_address);

        //Info getters
        std::string GetId() const;
        std::string GetName() const;
        std::string GetIp() const;
        int GetDeployedComponentCount() const;

        //Fully qualified endpoint getters
        std::string GetManagementEndpoint() const;
        std::string GetModelLoggerEndpoint() const;
        std::string GetOrbEndpoint() const;

        //Port number only getters
        std::string GetManagementPort() const;
        std::string GetModelLoggerPort() const;
        bool HasOrbPort() const;
        std::string GetOrbPort() const;
        std::vector<std::string> GetAllPublisherPorts() const;
        std::vector<std::string> GetAllLoggingPorts() const;

        //Port number setters
        void SetManagementPort(const std::string& management_port);
        void SetModelLoggerPort(const std::string& management_port);
        void SetOrbPort(const std::string& orb_port);

        //Node, component, attribute and logger adders
        void AddNode(std::unique_ptr<Node> node);
        void AddComponent(std::unique_ptr<Component> component);
        void AddAttribute();
        void AddLogger(std::unique_ptr<LoganServer> logger);
        void AddLogger(std::unique_ptr<LoganClient> logger);

        //Update requirement management
        void SetDirty();
        bool IsDirty();


        //Deep getters
        bool HasNode(const std::string& node_id) const;
        Node& GetNode(const std::string& node_id) const;
        bool HasComponent(const std::string& component_id) const;
        Component& GetComponent(const std::string& component_id) const;
        bool HasPort(const std::string& port_id) const;
        Port& GetPort(const std::string& port_id) const;

        //protobuf getters
        NodeManager::Node GetUpdate();
        NodeManager::Node GetFullProto();

    private:
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

        //logger id -> logan server
        std::unordered_map<std::string, std::unique_ptr<LoganServer> > logan_servers_;

        //logger id -> logan client
        std::unordered_map<std::string, std::unique_ptr<LoganClient> > logan_clients_;

};
};

#endif //ENVIRONMENT_MANAGER_NODE_H