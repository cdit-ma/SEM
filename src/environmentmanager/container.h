#ifndef ENVIRONMENTMANAGER_CONTAINER_H
#define ENVIRONMENTMANAGER_CONTAINER_H

#include <unordered_map>
#include "uniquequeue.hpp"
#include <memory>
#include <proto/controlmessage/controlmessage.pb.h>

namespace EnvironmentManager{
class Environment;
class Node;
class Component;
class Port;
class Logger;

class Container {
public:
    Container(Environment& environment, Node& parent, const NodeManager::Container& container);
    ~Container();

    std::string GetId();
    std::string GetName();

    Environment& GetEnvironment() const;
    Node& GetNode() const;

    bool HasComponent(const std::string& component_id);
    bool HasLogger(const std::string& logger_id);
    bool HasPort(const std::string& port_id);

    Component& GetComponent(const std::string& component_id);
    Logger& GetLogger(const std::string& logger_id);
    Port& GetPort(const std::string& port_id);

    void AddComponent(const NodeManager::Component& component_pb);
    EnvironmentManager::Logger& AddLogger(const NodeManager::Logger& logger_pb);

    int GetDeployedCount() const;
    int GetDeployedComponentCount() const;
    int GetLoganServerCount() const;
    std::vector<std::string> GetLoganServerIds() const;


            bool HasOrbPort() const;
    std::string GetOrbPort() const;
    std::string AssignOrbPort();
    void SetOrbPort(const std::string& orb_port);

    void SetNodeManagerMaster();
    bool IsNodeManagerMaster() const;

    bool IsLateJoiner() const;

    void SetDirty();
    bool IsDirty();

    std::unique_ptr<NodeManager::Container> GetProto(const bool full_update);

private:
    void AddModelLogger();
    EnvironmentManager::Logger& GetModelLogger();

    Environment& environment_;
    Node& parent_;

    std::string id_;
    std::string name_;
    std::string type_;

    std::string management_port_;
    std::string orb_port_;

    bool is_node_manager_master_ = false;

    bool dirty_;

    bool late_joiner_;

    std::unordered_map<std::string, std::unique_ptr<Component> > components_;
    std::unordered_map<std::string, std::unique_ptr<Logger> > loggers_;


};

}


#endif //PROJECT_CONTAINER_H
