#include "epmregistryimpl.h"
#include "grpc_util/server.h"
#include <boost/process.hpp>
#include <future>
#include <utility>
namespace sem::node_manager {

/// Constructs a string used to run EPM through boost::process::child
auto build_epm_start_command(const NodeConfig& node_config,
                             types::SocketAddress process_registration_endpoint,
                             types::SocketAddress experiment_manager_registration_endpoint,
                             types::Uuid experiment_uuid,
                             types::Uuid container_uuid) -> std::string
{
    auto epm_exe_path = node_config.find_epm_executable();
    // clang-format off
    return epm_exe_path +
            " --node_manager_uuid=" + node_config.uuid.to_string() +
            " --experiment_uuid=" + experiment_uuid.to_string() +
            " --experiment_manager_registration_endpoint=" + experiment_manager_registration_endpoint.to_string() +
            " --process_registration_endpoint=" + process_registration_endpoint.to_string() +
            " --container_uuid=" + container_uuid.to_string() +
            " --control_ip_address=" + node_config.control_ip_address.to_string() +
            " --data_ip_address=" + node_config.data_ip_address.to_string() +
            " --lib_root_dir=" + node_config.lib_root_dir +
            " --re_bin_dir=" + node_config.re_bin_path;
    // clang-format on
}

/// Starts a new ExperimentProcessManager with the given configuration details.
/// EPM will communicate directly with EnvironmentManager(ExperimentManager) for further, experiment
///  specific, configuration details.
/// NODISCARD, if EPMInfo is discarded epm process will be terminated as boost::process::child's
///  destructor calls process.terminate().
[[nodiscard]] auto
launch_epm_process(const NodeConfig& node_config,
                   types::Uuid experiment_uuid,
                   types::Uuid container_uuid) -> EpmRegistrarImpl::EpmRegistrationResult
{
    using EpmRegistrationResult = EpmRegistrarImpl::EpmRegistrationResult;
    // Create grpc service with std future to fill as arg.
    std::promise<EpmRegistrationResult> promise;
    std::future<EpmRegistrationResult> future = promise.get_future();

    auto epm_registrar = std::make_shared<EpmRegistrarImpl>(std::move(promise));

    // Start grpc server with service
    sem::grpc_util::Server server{node_config.control_ip_address, {epm_registrar}};

    // Get grpc server endpoint
    auto&& server_endpoint = server.endpoint();
    std::cout << server_endpoint.to_string() << std::endl;

    // Start epm process with args: registration server endpoint, container_uuid
    auto&& start_command = build_epm_start_command(
        node_config, server_endpoint, node_config.environment_manager_registration_endpoint,
        experiment_uuid, container_uuid);
    namespace bp = boost::process;

    auto epm_process_handle = std::make_unique<bp::child>(start_command);

    // Wait for our epm to send its registration message back to the epm_registrar. Return its
    // config info s.t. the environment manager knows where to find it.
    if(future.wait_for(EpmRegistryImpl::epm_start_duration_max) == std::future_status::timeout) {
        // Throw if our wait times out
        throw std::runtime_error("[EpmRegistry] - Timed out while waiting for EPM to register.");
    }

    auto registration_result = future.get();
    registration_result.process_handle = std::move(epm_process_handle);
    server.shutdown();
    return registration_result;
}

auto EpmRegistryImpl::start_epm(types::Uuid experiment_uuid, types::Uuid container_uuid)
    -> types::Uuid
{
    std::lock_guard lock(mutex_);
    std::cout << "[EpmRegistry] - Starting new epm for container:\n    " << container_uuid
              << "\n  for experiment:\n    " << experiment_uuid << std::endl;

    auto [epm_uuid, epm_info,
          epm_process_handle] = launch_epm_process(node_config_, experiment_uuid, container_uuid);

    std::cout << "[EpmRegistry] - Successfully STARTED EPM:\n    " << epm_uuid << std::endl;

    epm_info_.emplace(epm_uuid, epm_info);
    epm_process_handles_.emplace(epm_uuid, std::move(epm_process_handle));
    return epm_uuid;
}

auto EpmRegistryImpl::remove_epm(sem::types::Uuid uuid) -> void
{
    std::lock_guard lock(mutex_);
}

auto EpmRegistryImpl::get_epm_info(sem::types::Uuid uuid) -> EpmRegistry::EpmInfo
{
    std::lock_guard lock(mutex_);
    return epm_info_.at(uuid);
}
EpmRegistryImpl::EpmRegistryImpl(NodeConfig node_config) : node_config_{std::move(node_config)} {}

} // namespace sem::node_manager
