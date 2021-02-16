#ifndef RE_NODEMANAGER_H
#define RE_NODEMANAGER_H

#include "grpc_util/server.h"
#include "nodemanagerconfig.h"
#include <boost/process/child.hpp>
#include <memory>
namespace re::node_manager {

class NodeManager {
public:
    explicit NodeManager(NodeConfig config);
    auto wait() -> void;

private:
    const NodeConfig node_config_;
    std::optional<sem::types::SocketAddress> control_endpoint_;

    struct EpmRegistration {
        sem::types::Uuid request_uuid;
        sem::types::Uuid epm_uuid;
    };
};

} // namespace re::NodeManager

#endif // RE_NODEMANAGER_H
