#include "nodemanagerregistryimpl.h"
namespace sem::environment_manager {

auto NodeManagerRegistryImpl::add_node_manager(types::Uuid uuid, const NodeManagerInfo& info)
    -> void
{
    std::lock_guard lock(mutex_);
    auto [iter, insert_success] = node_manager_handles_.try_emplace(uuid, info);
    if(!insert_success) {
        throw std::invalid_argument("[NodeManagerRegistry] - Tried to register NodeManager that "
                                    "has "
                                    "already been registered. ("
                                    + uuid.to_string() + ")");
    }
}
auto NodeManagerRegistryImpl::remove_node_manager(types::Uuid nm_uuid) -> void
{
    std::lock_guard lock(mutex_);
    node_manager_handles_.erase(nm_uuid);
}
auto NodeManagerRegistryImpl::get_node_manager_info(types::Uuid nm_uuid) const -> NodeManagerInfo
{
    std::lock_guard lock(mutex_);
    return node_manager_handles_.at(nm_uuid);
}
} // namespace sem::environment_manager
