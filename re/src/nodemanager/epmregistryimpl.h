//
// Created by mitchell on 22/10/20.
//

#ifndef SEM_EPMREGISTRYIMPL_H
#define SEM_EPMREGISTRYIMPL_H
#include "epmregistry.h"
#include "nodemanagerconfig.h"
#include <mutex>
#include "epmregistrarimpl.h"

namespace sem::node_manager {
auto build_epm_start_command(const NodeConfig& node_config,
                             types::SocketAddress process_registration_endpoint,
                             types::SocketAddress experiment_manager_registration_endpoint,
                             types::Uuid experiment_uuid,
                             types::Uuid container_uuid) -> std::string;

[[nodiscard]] auto
launch_epm_process(const NodeConfig& node_config,
                   types::Uuid experiment_uuid,
                   types::Uuid container_uuid) -> EpmRegistrarImpl::EpmRegistrationResult;

class EpmRegistryImpl : public EpmRegistry {
public:
    EpmRegistryImpl(NodeConfig node_config);
    auto start_epm(types::Uuid experiment_uuid, types::Uuid container_uuid) -> types::Uuid final;
    auto remove_epm(sem::types::Uuid uuid) -> void final;
    auto get_epm_info(sem::types::Uuid uuid) -> EpmInfo final;

    static constexpr std::chrono::seconds epm_start_duration_max{5};

private:
    std::mutex mutex_;
    const NodeConfig node_config_;

    std::unordered_map<types::Uuid, EpmRegistry::EpmInfo> epm_info_;
    std::unordered_map<types::Uuid, EpmProcessHandle> epm_process_handles_;
};

} // namespace sem::node_manager
#endif // SEM_EPMREGISTRYIMPL_H
