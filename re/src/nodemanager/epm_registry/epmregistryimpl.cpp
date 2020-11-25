#include "epmregistryimpl.h"
#include "grpc_util/server.h"
#include <future>
#include <utility>
namespace sem::node_manager {

/// Starts a new ExperimentProcessManager with the given configuration details.
/// EPM will communicate directly with EnvironmentManager(ExperimentManager) for further, experiment
///  specific, configuration details.
/// NODISCARD, if EPMInfo is discarded epm process will be terminated as boost::process::child's
///  destructor calls process.terminate().
auto EpmRegistryImpl::start_epm(types::Uuid experiment_uuid, types::Uuid container_uuid)
    -> types::Uuid
{
    std::lock_guard lock(mutex_);
    std::cout << "[EpmRegistry] - Starting new epm for container:\n    " << container_uuid
              << "\n  for experiment:\n    " << experiment_uuid << std::endl;

    auto [epm_uuid, epm_info, epm_process_handle] =
        epm_registrar_service_->wait_on_epm_registration(
            {node_config_, experiment_uuid, container_uuid},
            epm_registrar_server_.endpoints().begin()->second);

    std::cout << "[EpmRegistry] - Successfully STARTED EPM:\n    " << epm_uuid << std::endl;

    epm_info_.emplace(epm_uuid, epm_info);
    epm_process_handles_.emplace(epm_uuid, std::move(epm_process_handle));
    return epm_uuid;
}

auto EpmRegistryImpl::remove_epm(sem::types::Uuid uuid) -> void
{
    std::lock_guard lock(mutex_);

    // TODO:
    //  Get a stub to the epm control service
    //  Send a shutdown request
    //  If graceful shutdown fails, call SIGTERM on child process.
    //   This is less than desirable, we shouldn't do this unless a polite shutdown call fails.
    std::error_code ec;
    epm_process_handles_.at(uuid)->terminate(ec);
    if(ec) {
        throw std::runtime_error("[EPMRegistry] - Failed to terminate child process with uuid: "
                                 + uuid.to_string());
    }
}

auto EpmRegistryImpl::get_epm_info(sem::types::Uuid uuid) -> EpmRegistry::EpmInfo
{
    std::lock_guard lock(mutex_);
    return epm_info_.at(uuid);
}
EpmRegistryImpl::EpmRegistryImpl(NodeConfig node_config) :
    node_config_{std::move(node_config)},
    epm_registrar_service_{std::make_shared<EpmRegistrarImpl>()},
    epm_registrar_server_(node_config.control_ip_address, {epm_registrar_service_})
{
}

} // namespace sem::node_manager
