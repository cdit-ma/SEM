//
// Created by mitchell on 22/10/20.
//

#ifndef SEM_EPMREGISTRYIMPL_H
#define SEM_EPMREGISTRYIMPL_H
#include "epmregistrarimpl.h"
#include "epmregistry.h"
#include "nodemanagerconfig.h"
#include <grpc_util/server.h>
#include <mutex>

namespace sem::node_manager::epm_registry {

class EpmRegistryImpl : public EpmRegistry {
public:
    EpmRegistryImpl(NodeConfig node_config);
    auto start_epm(types::Uuid experiment_uuid, types::Uuid container_uuid) -> types::Uuid final;
    auto remove_epm(sem::types::Uuid uuid) -> void final;
    auto get_epm_info(sem::types::Uuid uuid) -> EpmInfo final;

private:
    const NodeConfig node_config_;
    std::shared_ptr<EpmRegistrarImpl> epm_registrar_service_;
    grpc_util::Server epm_registrar_server_;

    std::mutex mutex_;
    std::unordered_map<types::Uuid, EpmInfo> epm_info_;
    std::unordered_map<types::Uuid, EpmProcessHandle> epm_process_handles_;
};

} // namespace sem::node_manager
#endif // SEM_EPMREGISTRYIMPL_H
