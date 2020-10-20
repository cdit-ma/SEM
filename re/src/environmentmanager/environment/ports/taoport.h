#ifndef ENVIRONMENT_MANAGER_TAO_PORT_H
#define ENVIRONMENT_MANAGER_TAO_PORT_H

#include "port.h"
#include "controlmessage.pb.h"
#include "socketaddress.hpp"

namespace re::EnvironmentManager {
namespace tao {
class Port : public EnvironmentManager::Port {
public:
    Port(Component& parent, const NodeManager::Port& port);
    Port(Experiment& parent, const NodeManager::ExternalPort& port);

protected:
    std::string GetOrbEndpoint() const;
    const std::string& GetServerKind() const;
    const std::string& GetNamingServiceEndpoint() const;
    const std::vector<std::string>& GetServerName() const;

    void FillPortPb(NodeManager::Port& port_pb) final;

private:
    std::vector<std::string> server_name_;
    std::string naming_service_endpoint_;

    // This is an unqualified representation of a tao endpoint. If ever converted to a string,
    //  always prepend with "iiop://"
    types::SocketAddress unqualified_orb_endpoint_;
    std::string server_kind_;
};
}; // namespace tao
}; // namespace re::EnvironmentManager

#endif // ENVIRONMENT_MANAGER_TAO_PORT_H