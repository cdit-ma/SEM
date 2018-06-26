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
class Component{
    public:
        Component(Environment& environment, Node& parent, const NodeManager::Component& component);
        void AddPort(const NodeManager::Port& port);
        void AddAttribute();

        std::string GetId();
        std::string GetName();

        Node& GetNode();
        Port& GetPort(const std::string& port_id);

        bool HasPort(const std::string& port_id);
        std::vector<std::string> GetAllPublisherPorts() const;

        void SetDirty();
        bool IsDirty();

        NodeManager::Component* GetUpdate();

    private:
        Environment& environment_;
        Node& node_;
        std::string id_;
        std::string name_;
        std::string type_;

        bool dirty_;

        std::unordered_map<std::string, std::unique_ptr<Port> > ports_;


};
};

#endif //ENVIRONMENT_MANAGER_COMPONENT_H