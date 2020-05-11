#ifndef ENVIRONMENT_MANAGER_ZMQ_PORT_H
#define ENVIRONMENT_MANAGER_ZMQ_PORT_H

#include <memory>
#include <mutex>
#include <proto/controlmessage/controlmessage.pb.h>
#include <types/socketaddress.hpp>

#include "port.h"

namespace re::EnvironmentManager {
class Environment;
class Experiment;
class Node;
class Component;
class Attribute;
namespace zmq {

class Port : public EnvironmentManager::Port {
public:
    Port(Component& parent, const NodeManager::Port& port);
    Port(Experiment& parent, const NodeManager::ExternalPort& port);
    ~Port();

protected:
    auto GetProducerEndpoint() const -> types::SocketAddress;
    void FillPortPb(NodeManager::Port& port_pb) final;

private:
    // REVIEW (Mitch): Optional needed here because
    std::optional<types::SocketAddress> producer_endpoint_;
    auto FillEndpointPublisher() -> std::optional<types::SocketAddress>;
    auto FillEndpointSubscriber(const NodeManager::ExternalPort& port)
        -> std::optional<types::SocketAddress>;
};
}; // namespace zmq
}; // namespace re::EnvironmentManager

#endif // ENVIRONMENT_MANAGER_COMPONENT_H