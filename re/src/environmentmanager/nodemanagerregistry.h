#ifndef SEM_NODEMANAGERREGISTRY_H
#define SEM_NODEMANAGERREGISTRY_H

#include "ipv4.hpp"
#include "socketaddress.hpp"

namespace sem::environment_manager {

struct NodeManagerInfo {
    types::Ipv4 control_ip_address;
    types::Ipv4 data_ip_address;
    types::SocketAddress control_endpoint;
    std::string hostname;
    std::string library_path;

    [[nodiscard]] auto get_control_stub() const
    {
        return grpc_util::get_stub<network::services::node_manager_control::NodeManagerControl>(
            control_endpoint);
    }
};

class NodeManagerRegistry {
public:
    virtual auto add_node_manager(types::Uuid, const NodeManagerInfo&) -> void = 0;
    virtual auto remove_node_manager(types::Uuid) -> void = 0;
    [[nodiscard]] virtual auto get_node_manager_info(types::Uuid) const -> NodeManagerInfo = 0;
};

} // namespace sem::environment_manager
#endif // SEM_NODEMANAGERREGISTRY_H
