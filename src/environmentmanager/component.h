#ifndef ENVIRONMENT_MANAGER_COMPONENT_H
#define ENVIRONMENT_MANAGER_COMPONENT_H

#include <mutex>
#include <unordered_map>
#include "uniquequeue.hpp"
#include <memory>
#include <re_common/proto/controlmessage/controlmessage.pb.h>

namespace EnvironmentManager{
class Environment;
class Node;
class Port;
class Attribute;
class Worker;

class Component{
    public:
        Component(Environment& environment, Node& parent, const NodeManager::Component& component);
        void AddAttribute();

        std::string GetId();
        std::string GetName();

        Port& GetPort(const std::string& port_id);
        Node& GetNode();
        Environment& GetEnvironment() const;

        bool HasPort(const std::string& port_id);

        void SetDirty();
        bool IsDirty();

        NodeManager::Component* GetUpdate();
        NodeManager::Component* GetProto();

    private:
        Environment& environment_;
        Node& node_;
        std::string id_;
        std::string name_;
        std::string type_;
        std::vector<std::string> namespaces_;

        bool dirty_;

        std::unordered_map<std::string, std::unique_ptr<Port> > ports_;
        std::unordered_map<std::string, std::unique_ptr<Worker> > workers_;
        std::unordered_map<std::string, std::unique_ptr<Attribute> > attributes_;

};
};

#endif //ENVIRONMENT_MANAGER_COMPONENT_H