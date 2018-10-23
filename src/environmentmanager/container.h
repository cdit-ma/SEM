#ifndef PROJECT_CONTAINER_H
#define PROJECT_CONTAINER_H

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

    std::string GetId();
    std::string GetName();

    Environment& GetEnvironment() const;
    Node& GetNode() const;

    bool HasComponent(const std::string& component_id);
    bool HasLogger(const std::string& logger_id);
    bool HasPort(const std::string& port_id);

    Component& GetComponent(const std::string& component_id);
    Port& GetPort(const std::string& port_id);


    int GetDeployedCount() const;
    int GetDeployedComponentCount() const;
    int GetLoganServerCount() const;

    void SetDirty();
    bool IsDirty();

    std::unique_ptr<NodeManager::Container> GetProto(const bool full_update);

private:
    void AddComponent(const NodeManager::Component& component_pb);
    void AddLogger(const NodeManager::Logger& logger_pb);

    Environment& environment_;
    Node& parent_;

    std::string id_;
    std::string name_;
    std::string type_; //??

    bool dirty_;

    std::unordered_map<std::string, std::unique_ptr<Component> > components_;
    std::unordered_map<std::string, std::unique_ptr<Logger> > loggers_;


};

}


#endif //PROJECT_CONTAINER_H
