#ifndef ENVIRONMENT_MANAGER_PORT_H
#define ENVIRONMENT_MANAGER_PORT_H

#include <mutex>
#include <unordered_map>
#include "uniquequeue.hpp"
#include <memory>
#include <proto/controlmessage/controlmessage.pb.h>

namespace EnvironmentManager{
class Environment;
class Component;
class Port{
    public:
        enum class Kind{
            NoKind = 0,
            Periodic = 1,
            Publisher = 2,
            Subscriber = 3,
            Requester = 4,
            Replier = 5,
        };
        enum class Middleware{
            NoMiddleware = 0,
            Zmq = 1,
            Rti = 2,
            Ospl = 3,
            Qpid = 4,
            Tao = 5,
        };
        Port(Environment& environment, Component& parent, const NodeManager::Port& port);
        ~Port();
        std::string GetName() const;
        std::string GetId() const;
        Port::Kind GetKind() const;
        Port::Middleware GetMiddleware() const;
        std::string GetPublisherPort() const;
        std::string GetTopic() const;
        Component& GetComponent() const;
        Node& GetNode() const;
        Experiment& GetExperiment() const;

        void SetType(const std::string& type);
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

        Kind ProtoPortKindToInternal(NodeManager::Port::Kind kind);
        NodeManager::Port::Kind InternalPortKindToProto(Port::Kind middleware);
        Middleware ProtoPortMiddlewareToInternal(NodeManager::Middleware middleware);
        NodeManager::Middleware InternalPortMiddlewareToProto(Port::Middleware middleware);



    private:
        Environment& environment_;
        Component& component_;
        std::string id_;
        std::string name_;
        std::string type_;
        Kind kind_;
        Middleware middleware_;

        bool dirty_;

        std::string topic_name_;

        std::set<std::string> endpoints_;

        std::string publisher_port_;

        std::set<std::string> connected_port_ids_;
        std::set<std::string> connected_external_port_ids_;

};
};

#endif //ENVIRONMENT_MANAGER_COMPONENT_H