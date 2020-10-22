#ifndef RE_NODEMANAGERREGISTRY_H
#define RE_NODEMANAGERREGISTRY_H

#include "grpc_util/grpc_util.h"
#include "ipv4.hpp"
#include "node_manager_control_service.grpc.pb.h"
#include "socketaddress.hpp"
#include "uuid.h"
#include "nodemanagerregistry.h"

namespace sem::environment_manager {

class NodeManagerRegistryImpl : public NodeManagerRegistry {
public:
    auto add_node_manager(types::Uuid uuid, const NodeManagerInfo& info) -> void final;
    auto remove_node_manager(types::Uuid nm_uuid) -> void final;
    [[nodiscard]] auto get_node_manager_info(types::Uuid nm_uuid) const -> NodeManagerInfo final;

private:
    mutable std::mutex mutex_;
    std::unordered_map<types::Uuid, NodeManagerInfo> node_manager_handles_;
};
} // namespace sem::environment_manager

#endif // RE_NODEMANAGERREGISTRY_H
