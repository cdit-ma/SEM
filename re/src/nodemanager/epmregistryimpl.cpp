//
// Created by mitchell on 22/10/20.
//

#include "epmregistryimpl.h"
#include "epmregistrarimpl.h"
#include "grpc_util/server.h"
#include <future>
namespace sem::node_manager {
namespace detail {

/// Finds EPM executable at path specified. Not using std::filesystem::path as some compilers don't
///  yet support it.
/// Throws if EPM executable is not found.
auto find_epm_executable(const std::string& re_bin_path) -> std::string
{
    namespace bp = boost::process;
    auto epm_exe_path = bp::search_path("experiment_process_manager", {re_bin_path});
    if(epm_exe_path.empty()) {
        throw std::runtime_error("Experiment process manager executable not found in path: "
                                 + re_bin_path);
    }
    return epm_exe_path.string();
}

/// Constructs a string used to run EPM through boost::process::child
auto build_epm_start_command(const NodeConfig& node_config, sem::types::Uuid experiment_uuid)
    -> std::string
{
    auto epm_exe_path = find_epm_executable(node_config.re_bin_path);
    return epm_exe_path + " --experiment_uuid=" + experiment_uuid.to_string()
           + " --environment_manager_registration_endpoint="
           + node_config.environment_manager_registration_endpoint.to_string()
           + " --ip_address=" + node_config.control_ip_address.to_string() + " --lib_root_dir="
           + node_config.lib_root_dir + " --re_bin_dir=" + node_config.re_bin_path
           + " --registration_entity_uuid=" + node_config.uuid.to_string();
}
/// Starts a new ExperimentProcessManager with the given configuration details.
/// EPM will communicate directly with EnvironmentManager(ExperimentManager) for further, experiment
///  specific, configuration details.
auto new_epm(const sem::types::Uuid& experiment_uuid, const std::string& experiment_name) -> void
{
    std::cout << "[NodeManager] - Starting new epm for experiment:\n    " << experiment_name << " ("
              << experiment_uuid << ")" << std::endl;

    namespace bp = boost::process;
    auto&& request_uuid = sem::types::Uuid{};
    auto&& epm_start_command = detail::build_epm_start_command(node_config_, experiment_uuid);
    std::make_unique<bp::child>(epm_start_command)
        // Wait for our epm to send its registration message back to the node manager. Return its
        // uuid s.t. the environment manager knows where to find it.
        auto epm_uuid = WaitForEpmRegistrationMessage(request_uuid);
    std::cout << "[NodeManager] - Successfully STARTED epm:\n    (" << epm_uuid << ")" << std::endl;
    return epm_uuid;
}
auto start_epm(types::Uuid experiment_uuid, types::Uuid container_uuid) -> EpmRegistry::EpmInfo
{
    // Create grpc service with std future to fill as arg.
    std::promise<EpmRegistry::EpmInfo> promise;
    std::future<EpmRegistry::EpmInfo> future = promise.get_future();

    auto epm_registrar = std::make_shared<EpmRegistrarImpl>(std::move(promise));

    // Start grpc server with service
    sem::grpc_util::Server server{types::Ipv4::localhost(), {epm_registrar}};

    // Get grpc server endpoint
    auto server_endpoint = server.endpoint();

    // Start epm process with args: registration server endpoint, container_uuid

    // Wait till we've had our promise filled by the grpc server/service
    future.wait_for(EpmRegistryImpl::epm_start_duration_max);
    auto epm_info = future.get();
    server.shutdown();
    return epm_info;
};
} // namespace detail

auto EpmRegistryImpl::start_epm() -> void {}

auto EpmRegistryImpl::remove_epm(sem::types::Uuid uuid) -> void
{
    std::lock_guard lock(mutex_);
}

auto EpmRegistryImpl::get_epm_info(sem::types::Uuid uuid) -> EpmRegistry::EpmInfo
{
    std::lock_guard lock(mutex_);
}

} // namespace sem::node_manager
