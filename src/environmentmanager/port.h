#ifndef ENVIRONMENT_MANAGER_PORT_H
#define ENVIRONMENT_MANAGER_PORT_H

#include <mutex>
#include <unordered_map>
#include "uniquequeue.hpp"
#include <memory>
#include <proto/controlmessage/controlmessage.pb.h>

namespace EnvironmentManager{
class Component;
class Port{
    public:
        enum class PortKind{
            NoKind = 0,
            Periodic = 1,
            Publisher = 2,
            Subscriber = 3,
            Requester = 4,
            Replier = 5,
        };
        enum class PortMiddleware{
            NoMiddleware = 0,
            Zmq = 1,
            Rti = 2,
            Ospl = 3,
            Qpid = 4,
            Tao = 5,
        };
        Port(const Component& parent, const std::string& id, const std::string& name, Port::PortKind kind, Port::PortMiddleware middleware);
        std::string GetName() const;
        std::string GetId() const;
        Port::PortKind GetKind() const;
        Port::PortMiddleware GetMiddleware() const;
        std::string GetPublisherPort() const;
        std::string GetTopic() const;
        Component& GetParent() const;

        void SetPublisherPort(const std::string& publisher_port);
        void SetTopic(const std::string& topic_name);
        void AddEndpoint(const std::string& endpoint);
        void RemoveEndpoint(const std::string& endpoint);
        void AddConnectedPortId(const std::string& port_id);
        void AddExternalConnectedPortId(const std::string& port_id);

        bool IsConnectedTo(const std::string& port_id) const;

        void SetDirty();
        bool IsDirty() const;

        NodeManager::Port* GetUpdate();

    private:
        const Component& parent_;
        std::string id_;
        std::string name_;
        PortKind kind_;
        PortMiddleware middleware_;

        bool dirty_;

        std::string topic_name_;

        std::set<std::string> endpoints_;

        std::string publisher_port_;

        std::set<std::string> connected_port_ids_;
        std::set<std::string> connected_external_port_ids_;

};
};

#endif //ENVIRONMENT_MANAGER_COMPONENT_H