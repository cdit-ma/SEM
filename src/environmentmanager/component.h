#ifndef ENVIRONMENT_MANAGER_COMPONENT_H
#define ENVIRONMENT_MANAGER_COMPONENT_H

#include <mutex>
#include <unordered_map>
#include "uniquequeue.hpp"
#include <memory>
#include <proto/controlmessage/controlmessage.pb.h>

namespace EnvironmentManager{
class Node;
class Port;
class Component{
    public:
        Component(const Node& parent, const std::string& id, const std::string& name);
        void AddPort(std::unique_ptr<Port> node);
        void AddAttribute();

        std::string GetId();
        std::string GetName();

        Node& GetParent();

        std::vector<std::string> GetAllPublisherPorts() const;

        void SetDirty();
        bool IsDirty();

        NodeManager::Component* GetUpdate();

    private:
        const Node& parent_;
        std::string id_;
        std::string name_;
        std::string type_;

        bool dirty_;

        std::unordered_map<std::string, std::unique_ptr<Port> > ports_;


};
};

#endif //ENVIRONMENT_MANAGER_COMPONENT_H