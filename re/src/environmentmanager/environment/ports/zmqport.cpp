#include "zmqport.h"
#include "../component.h"
#include "../environment.h"
#include "../node.h"
#include "helper.h"

namespace re::EnvironmentManager {

EnvironmentManager::zmq::Port::Port(Component& parent, const NodeManager::Port& port) :
    EnvironmentManager::Port(parent, port), producer_endpoint_{FillEndpointPublisher()}
{
}

auto EnvironmentManager::zmq::Port::FillEndpointPublisher() -> std::optional<sem::types::SocketAddress>
{
    if(kind_ == EnvironmentManager::Port::Kind::Publisher
       || kind_ == EnvironmentManager::Port::Kind::Replier) {
        auto ip = GetNode().GetIp();
        return GetEnvironment().GetEndpoint(ip);
    }
    return std::nullopt;
}

EnvironmentManager::zmq::Port::Port(Experiment& parent, const NodeManager::ExternalPort& port) :
    EnvironmentManager::Port(parent, port), producer_endpoint_{FillEndpointSubscriber(port)}
{
}

auto EnvironmentManager::zmq::Port::FillEndpointSubscriber(const NodeManager::ExternalPort& port)
    -> std::optional<sem::types::SocketAddress>
{
    if(GetBlackboxType() == BlackboxType::PubSub) {
        return sem::types::SocketAddress(
            NodeManager::GetAttribute(port.attributes(), "publisher_address").s(0));
    } else if(GetBlackboxType() == BlackboxType::ReqRep) {
        return sem::types::SocketAddress(
            NodeManager::GetAttribute(port.attributes(), "server_address").s(0));
    }
    return std::nullopt;
}

EnvironmentManager::zmq::Port::~Port()
{
    if(!IsBlackbox()) {
        if(kind_ == Kind::Publisher || kind_ == Kind::Replier) {
            GetEnvironment().FreeEndpoint(GetProducerEndpoint());
        }
    }
}

auto EnvironmentManager::zmq::Port::GetProducerEndpoint() const -> sem::types::SocketAddress
{
    return producer_endpoint_.value();
}

void EnvironmentManager::zmq::Port::FillPortPb(NodeManager::Port& port_pb)
{
    const auto& port_kind = GetKind();

    auto attrs = port_pb.mutable_attributes();

    if(port_kind == Kind::Publisher) {
        NodeManager::SetStringAttribute(attrs, "publisher_address", GetProducerEndpoint().tcp());
    } else if(port_kind == Kind::Replier) {
        NodeManager::SetStringAttribute(attrs, "server_address", GetProducerEndpoint().tcp());
    } else if(port_kind == Kind::Subscriber) {
        std::vector<std::string> publisher_endpoints;
        // Connect all Internal ports
        for(const auto& port_ref : GetConnectedPorts()) {
            const auto& port = port_ref.get();
            if(port.GetMiddleware() == EnvironmentManager::Port::Middleware::Zmq) {
                const auto& zmq_port = (const Port&)port;
                publisher_endpoints.push_back(zmq_port.GetProducerEndpoint().tcp());
            }
        }
        NodeManager::SetStringListAttribute(attrs, "publisher_addresses", publisher_endpoints);
    } else if(port_kind == Kind::Requester) {
        // Connect all Internal ports
        std::vector<std::string> replier_endpoints;
        for(const auto& port_ref : GetConnectedPorts()) {
            const auto& port = port_ref.get();
            if(port.GetMiddleware() == EnvironmentManager::Port::Middleware::Zmq) {
                const auto& zmq_port = (const Port&)port;
                NodeManager::SetStringAttribute(attrs, "server_address",
                                                GetProducerEndpoint().tcp());
                break;
            }
        }
    }
}
} // namespace re::EnvironmentManager
