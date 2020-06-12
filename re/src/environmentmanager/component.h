#ifndef ENVIRONMENT_MANAGER_COMPONENT_H
#define ENVIRONMENT_MANAGER_COMPONENT_H

#include <mutex>
#include <unordered_map>
#include "uniquequeue.hpp"
#include <memory>
#include <proto/controlmessage/controlmessage.pb.h>

namespace EnvironmentManager{
class Environment;
class Node;
class Port;
class Attribute;
class Container;
class Worker;

class Component{
    public:
        Component(Environment& environment, Container& parent, const NodeManager::Component& component);
        void AddAttribute();

        std::string GetId();
        std::string GetName();

        Port& GetPort(const std::string& port_id);
        Container& GetContainer() const;
        Node& GetNode() const;
        Environment& GetEnvironment() const;

        bool HasPort(const std::string& port_id);

        void SetDirty();
        bool IsDirty();

        std::unique_ptr<NodeManager::Component> GetProto(const bool full_update);
    private:
        Environment& environment_;
        Container& parent_;
        std::string id_;
        std::string name_;
        std::string type_;
        std::vector<std::string> namespaces_;

        bool dirty_;

        std::unordered_map<std::string, std::unique_ptr<Port> > ports_;
        std::unordered_map<std::string, std::unique_ptr<Worker> > workers_;
        std::unordered_map<std::string, std::unique_ptr<Attribute> > attributes_;

        std::vector<std::string> parent_stack_;
        std::vector<int> replication_indices_;

};
};

#endif //ENVIRONMENT_MANAGER_COMPONENT_H