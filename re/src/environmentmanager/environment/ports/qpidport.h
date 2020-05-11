#ifndef ENVIRONMENT_MANAGER_QPID_PORT_H
#define ENVIRONMENT_MANAGER_QPID_PORT_H

#include <memory>
#include <mutex>
#include <proto/controlmessage/controlmessage.pb.h>
#include <types/socketaddress.hpp>

#include "port.h"

namespace re::EnvironmentManager{
class Environment;
class Experiment;
class Node;
class Component;
class Attribute;
namespace qpid {

class Port : public EnvironmentManager::Port{
    public:
        Port(Component& parent, const NodeManager::Port& port);
        Port(Experiment& parent, const NodeManager::ExternalPort& port);
        Port(const types::Uuid& event_uuid,
             Component& parent,
             const Representation::ExperimentDefinition& experiment_definition,
             const Representation::MiddlewarePortInstance& port);

    protected:
        types::SocketAddress GetBrokerAddress() const;
        std::string GetTopic() const;
        void FillPortPb(NodeManager::Port& port_pb) final;

    private:
        std::string topic_name_;
        types::SocketAddress broker_address_;
};
};
};

#endif //ENVIRONMENT_MANAGER_COMPONENT_H