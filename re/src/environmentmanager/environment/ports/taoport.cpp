#include "taoport.h"
#include "component.h"
#include "container.h"
#include "environment.h"
#include "node.h"
#include "helper.h"

namespace re::EnvironmentManager {

tao::Port::Port(Component& parent, const NodeManager::Port& port) :
    EnvironmentManager::Port(parent, port),
    unqualified_orb_endpoint_(GetComponent().GetContainer().GetUnqualifiedOrbEndpoint())
{
    // Assign port to this
    auto& node = GetNode();

    // Set the server name
    const auto& experiment_name = GetExperiment().GetName();
    const auto& component_name = parent.GetName() + "_" + parent.GetId();
    const auto& server_name = GetName() + "_" + GetId();
    server_name_.push_back(experiment_name);
    server_name_.push_back(component_name);
    server_name_.push_back(server_name);

    // Set the naming service endpoint
    naming_service_endpoint_ = GetEnvironment().GetTaoNamingServiceAddress();
    if(GetKind() == Kind::Replier) {
        server_kind_ = "medea_port";
    }
}

tao::Port::Port(Experiment& parent, const NodeManager::ExternalPort& port) :
    EnvironmentManager::Port(parent, port),
    // XXX: Horrible hacky fix
    unqualified_orb_endpoint_{types::SocketAddress::from_string("0.0.0.0:0000")}
{
    naming_service_endpoint_ =
        NodeManager::GetAttribute(port.attributes(), "naming_server_endpoint").s(0);
    auto& attr = NodeManager::GetAttribute(port.attributes(), "server_name");
    server_name_.insert(server_name_.end(), attr.s().begin(), attr.s().end());

    server_kind_ = NodeManager::GetAttribute(port.attributes(), "server_kind").s(0);

    if(server_name_.empty()) {
        throw std::runtime_error("TAO External Port requires at least one server_name");
    }

    if(naming_service_endpoint_.empty()) {
        throw std::runtime_error("TAO External Port a qualified naming_server_endpoint");
    }

    if(server_kind_.empty()) {
        throw std::runtime_error("TAO External Port requires a server_kind.");
    }
}

const std::vector<std::string>& tao::Port::GetServerName() const
{
    return server_name_;
}

const std::string& tao::Port::GetServerKind() const
{
    return server_kind_;
}

const std::string& tao::Port::GetNamingServiceEndpoint() const
{
    return naming_service_endpoint_;
}

std::string tao::Port::GetOrbEndpoint() const
{
    return "iiop://" + unqualified_orb_endpoint_.to_string();
}

void tao::Port::FillPortPb(NodeManager::Port& port_pb)
{
    auto attrs = port_pb.mutable_attributes();

    if(!IsBlackbox()) {
        NodeManager::SetStringAttribute(attrs, "orb_endpoint", GetOrbEndpoint());
    }

    // If we're a replier, we can set our server name(topic) and naming service endpoint based on
    // internal information.
    if(GetKind() == Kind::Replier) {
        // Set the Server Name Attribute
        std::vector<std::string> server_name = GetServerName();
        std::string naming_service_endpoint = GetNamingServiceEndpoint();

        std::set<std::vector<std::string>> blackbox_server_names;
        std::set<std::string> blackbox_naming_service_endpoints;

        for(const auto& connected_port_ref : GetConnectedPorts()) {
            const auto& connected_port = connected_port_ref.get();
            const auto& tao_port = (const Port&)connected_port;
            if(connected_port.IsBlackbox()) {
                blackbox_naming_service_endpoints.insert(tao_port.GetNamingServiceEndpoint());
                blackbox_server_names.insert(tao_port.GetServerName());
            }
        }

        if(!blackbox_server_names.empty()) {
            if(blackbox_server_names.size() > 1) {
                std::cerr << "* Experiment[" << GetExperiment().GetName()
                          << "]: Has multiple TAO blackbox server names connected to Port: '"
                          << GetId() << "'" << std::endl;
            }
            server_name = *(blackbox_server_names.begin());
            std::cerr << "* Experiment[" << GetExperiment().GetName()
                      << "]: Has blackbox connected to port: '" << GetId()
                      << "'. Overriding server name!" << std::endl;
        }

        if(!blackbox_naming_service_endpoints.empty()) {
            if(blackbox_naming_service_endpoints.size() > 1) {
                std::cerr << "* Experiment[" << GetExperiment().GetName()
                          << "]: Has multiple blackbox naming server endpoints connected to Port: '"
                          << GetId() << "'" << std::endl;
            }
            naming_service_endpoint = *(blackbox_naming_service_endpoints.begin());
            std::cerr << "* Experiment[" << GetExperiment().GetName()
                      << "]: Has blackbox connected to port: '" << GetId()
                      << "'. Overriding naming service endpoint!" << std::endl;
        }

        NodeManager::SetStringListAttribute(attrs, "server_name", server_name);
        NodeManager::SetStringAttribute(attrs, "server_kind", server_kind_);
        NodeManager::SetStringAttribute(attrs, "naming_service_endpoint", naming_service_endpoint);
    }

    else if(GetKind() == Kind::Requester) {
        // Connect all Internal ports
        for(const auto& connected_port_ref : GetConnectedPorts()) {
            const auto& connected_port = connected_port_ref.get();
            const auto& tao_port = (const Port&)connected_port;

            NodeManager::SetStringListAttribute(attrs, "server_name", tao_port.GetServerName());
            NodeManager::SetStringAttribute(attrs, "server_kind", tao_port.GetServerKind());
            NodeManager::SetStringAttribute(attrs, "naming_service_endpoint",
                                            tao_port.GetNamingServiceEndpoint());
            break;
        }
    }
}
} // namespace re::EnvironmentManager