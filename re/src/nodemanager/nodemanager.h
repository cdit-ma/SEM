#ifndef RE_NODEMANAGER_H
#define RE_NODEMANAGER_H

#include "epmregistrarimpl.h"
#include "epmregistry.h"
#include "grpc_util/server.h"
#include "nodemanager_config/nodemanagerconfig.h"
#include "nodemanagercontrolimpl.h"
#include <memory>
namespace sem::node_manager {

auto register_node_manager(const NodeConfig& config, sem::types::SocketAddress control_endpoint)
    -> void;

auto deregister_node_manager(const NodeConfig& config) -> void;

class NodeManager {
public:
    explicit NodeManager(NodeConfig config, epm_registry::EpmRegistry& epm_registry);
    ~NodeManager();
    auto wait() -> void;
    auto shutdown() -> void;

private:
    // Data member order matters as we initialise most data members in class' init list
    const NodeConfig node_config_;
    epm_registry::EpmRegistry& epm_registry_;

    std::shared_ptr<NodeManagerControlImpl> control_service_;

    sem::grpc_util::Server server_;
};

} // namespace sem::node_manager

#endif // RE_NODEMANAGER_H
