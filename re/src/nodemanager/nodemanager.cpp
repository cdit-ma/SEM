#include "nodemanager.h"
#include "grpc_util/grpc_util.h"
#include "node_manager_registration_service.grpc.pb.h"
#include "nodemanagerconfig.h"
#include <boost/process/search_path.hpp>
#include <fstream>
#include <utility>

namespace re::node_manager {
namespace detail {

/// Tries to register this NodeManager with the EnvironmentManager.
/// Throws on any sort of registration failure.
auto RegisterNodeManager(const NodeConfig& config, sem::types::SocketAddress control_endpoint)
    -> void
{
    using namespace sem::network::services::node_manager_registration;
    auto registration_stub = sem::grpc_util::get_stub<NodeManagerRegistrar>(
        config.environment_manager_registration_endpoint);

    RegistrationRequest request;
    request.set_uuid(config.uuid.to_string());
    request.set_data_ip_address(config.data_ip_address.to_string());
    request.set_control_ip_address(config.control_ip_address.to_string());
    request.set_node_manager_control_endpoint(control_endpoint.to_string());
    if(config.hostname) {
        request.set_hostname(*config.hostname);
    }
    RegistrationReply reply;
    grpc::ClientContext context;
    auto status = registration_stub->RegisterNodeManager(&context, request, &reply);
    if(!status.ok()) {
    }
}

/// Finds EPM executable at path specified. Not using std::filesystem::path as some compilers don't
///  yet support it.
/// Throws if EPM executable is not found.
auto FindEpmExecutable(const std::string& re_bin_path) -> std::string
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
auto BuildEpmStartCommand(const NodeConfig& node_config,
                          sem::types::Uuid experiment_uuid,
                          sem::types::Uuid creation_request_id) -> std::string
{
    auto epm_exe_path = FindEpmExecutable(node_config.re_bin_path);
    return epm_exe_path + " --experiment_uuid=" + experiment_uuid.to_string()
           + " --creation_request_uuid=" + creation_request_id.to_string()
           + " --environment_manager_registration_endpoint="
           + node_config.environment_manager_registration_endpoint.to_string()
           + " --ip_address=" + node_config.control_ip_address.to_string() + " --lib_root_dir="
           + node_config.lib_root_dir + " --re_bin_dir=" + node_config.re_bin_path
           + " --registration_entity_uuid=" + node_config.uuid.to_string();
}
} // namespace detail

NodeManager::NodeManager(NodeConfig config) :
    node_config_{std::move(config)}
//    control_server_{node_config_.control_ip_address, {/* TODO: services go here*/}}
{
    // Test that we can find the EPM executable before doing anything else.
    // Fail early my dudes.
    detail::FindEpmExecutable(node_config_.re_bin_path);

    std::cout << "[NodeManager] - Started with uuid: (" << node_config_.uuid << ")" << std::endl;

    // Start Control service

    control_endpoint_ = sem::types::SocketAddress(node_config_.control_ip_address, 45000);

    // Start Epm registration service

    // We now have all of our threads/repliers set up so we can register with the env manager.
    // Register with environment manager. Throws on failure.
    detail::RegisterNodeManager(node_config_, *control_endpoint_);
    std::cout << "[NodeManager] - Registered with environment manager at:\n    ("
              << node_config_.environment_manager_registration_endpoint.to_string() << ")"
              << std::endl;
}

/// Starts a new ExperimentProcessManager with the given configuration details.
/// EPM will communicate directly with EnvironmentManager(ExperimentManager) for further, experiment
///  specific, configuration details.
auto NodeManager::NewEpm(const sem::types::Uuid& experiment_uuid,
                         const std::string& experiment_name) -> sem::types::Uuid
{
    std::cout << "[NodeManager] - Starting new epm for experiment:\n    " << experiment_name << " ("
              << experiment_uuid << ")" << std::endl;

    namespace bp = boost::process;
    auto&& request_uuid = sem::types::Uuid{};
    auto&& epm_start_command = detail::BuildEpmStartCommand(node_config_, experiment_uuid,
                                                            request_uuid);

    // Wait for our epm to send its registration message back to the node manager. Return its
    // uuid s.t. the environment manager knows where to find it.
    auto epm_uuid = WaitForEpmRegistrationMessage(request_uuid);
    epm_process_handles_.emplace(epm_uuid, std::make_unique<bp::child>(epm_start_command));
    std::cout << "[NodeManager] - Successfully STARTED epm:\n    (" << epm_uuid << ")" << std::endl;
    return epm_uuid;
}

/// Waits on a semaphore that is notified in the epm registration replier thread that runs:
///  HandleEpmRegistration
auto NodeManager::WaitForEpmRegistrationMessage(const sem::types::Uuid& request_uuid)
    -> sem::types::Uuid
{
    // How long we should wait for the EPM to start
    // TODO: Tune this number?
    auto registration_wait_timeout = std::chrono::milliseconds(1000);

    std::unique_lock lock{epm_registration_mutex_};
    if(epm_registration_semaphore_.wait_for(lock, registration_wait_timeout, [this, request_uuid]() {
           // Check that we have a registration to process and
           // that the registration id matches.
           //  XXX: This could be a problem?
           return !epm_registrations_.empty()
                  && epm_registrations_.front().request_uuid == request_uuid;
       })) {
        auto epm_uuid = epm_registrations_.front().epm_uuid;
        epm_registrations_.pop();

        return epm_uuid;
    } else {
        throw std::runtime_error("EPM startup timed out.");
    }
}

/// Used as callback for the NodeManager's EPM registration listener.
auto NodeManager::HandleEpmRegistration(const EpmRegistrationRequest& request)
    -> EpmRegistrationReply
{
    // TODO: Build list of pending registration uuids. Check against that before we notify.
    auto request_uuid = sem::types::Uuid{request.request_uuid()};
    auto epm_uuid = sem::types::Uuid{request.epm_uuid()};
    {
        std::lock_guard lock{epm_registration_mutex_};
        epm_registrations_.push({request_uuid, epm_uuid});
    }
    epm_registration_semaphore_.notify_one();
    EpmRegistrationReply reply;
    reply.set_success(true);
    return reply;
}

/// Hard terminates process running EPM. Uses SIGTERM
/// Throws on EPM uuid not found.
void NodeManager::StopEpm(sem::types::Uuid uuid)
{
    if(epm_process_handles_.count(uuid) != 1) {
        throw std::runtime_error("Could not find epm to stop.");
    }
    epm_process_handles_.at(uuid)->terminate();
    epm_process_handles_.erase(uuid);

    std::cout << "[NodeManager] - Successfully STOPPED epm:\n    (" << uuid << ")" << std::endl;
}
auto NodeManager::wait() -> void
{
//    control_server_.wait();
}

} // namespace re::NodeManager
