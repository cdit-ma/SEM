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
        void AddNode(std::unique_ptr<Node> node);
        void AddComponent(std::unique_ptr<Component> component);
        void AddAttribute();
        void AddLogger(std::unique_ptr<LoganServer> logger);
        void AddLogger(std::unique_ptr<LoganClient> logger);

        std::string GetId();
        std::string GetName();
        std::string GetIp();

        void SetDirty();
        bool IsDirty();

        NodeManager::Node GetUpdate();

    private:
        std::string id_;
        std::string name_;
        std::string ip_;

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