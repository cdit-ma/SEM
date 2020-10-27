#ifndef RE_NODEMANAGER_H
#define RE_NODEMANAGER_H

#include "epmregistrarimpl.h"
#include "epmregistry.h"
#include "grpc_util/server.h"
#include "nodemanagerconfig.h"
#include "nodemanagercontrolimpl.h"
#include <memory>
namespace sem::node_manager {

class NodeManager {
public:
    explicit NodeManager(NodeConfig config, EpmRegistry& epm_registry);
    ~NodeManager();
    auto wait() -> void;
    auto shutdown() -> void;

private:
    // Data member order matters as we initialise most data members in class' init list
    const NodeConfig node_config_;
    EpmRegistry& epm_registry_;

    std::shared_ptr<NodeManagerControlImpl> control_service_;

    sem::grpc_util::Server server_;
};

} // namespace sem::node_manager

#endif // RE_NODEMANAGER_H
