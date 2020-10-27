#include "nodemanager.h"
#include "grpc_util/grpc_util.h"
#include "node_manager_registration_service.grpc.pb.h"
#include "nodemanagerconfig.h"
#include <boost/process/search_path.hpp>
#include <fstream>
#include <utility>

namespace sem::node_manager {
namespace detail {

/// Tries to register this NodeManager with the EnvironmentManager.
/// Throws on any sort of registration failure.
auto register_node_manager(const NodeConfig& config, sem::types::SocketAddress control_endpoint)
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
    RegistrationResponse reply;
    grpc::ClientContext context;
    auto status = registration_stub->RegisterNodeManager(&context, request, &reply);
    if(!status.ok()) {
        throw std::runtime_error("[NodeManager] - Failed to connect to environment manager. gRPC "
                                 "call cancelled.");
    }
    if(!reply.success()) {
        throw std::runtime_error("[NodeManager] - Failed to connect to environment manager.\n    "
                                 + reply.message());
    }
}

auto deregister_node_manager(const NodeConfig& config)
{
    using namespace sem::network::services::node_manager_registration;
    auto registration_stub = sem::grpc_util::get_stub<NodeManagerRegistrar>(
        config.environment_manager_registration_endpoint);

    DeregistrationRequest request;
    request.set_uuid(config.uuid.to_string());

    DeregistrationResponse reply;
    grpc::ClientContext context;
    auto status = registration_stub->DeregisterNodeManager(&context, request, &reply);
    if(!status.ok()) {
        throw std::runtime_error("[NodeManager] - Failed to deregister node manager. gRPC "
                                 "call cancelled.");
    }
    if(!reply.success()) {
        throw std::runtime_error("[NodeManager] - Failed to deregister node manager.\n    "
                                 + reply.message());
    }
}

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
auto BuildEpmStartCommand(const NodeConfig& node_config,
                          sem::types::Uuid experiment_uuid,
                          sem::types::Uuid creation_request_id) -> std::string
{
    auto epm_exe_path = find_epm_executable(node_config.re_bin_path);
    return epm_exe_path + " --experiment_uuid=" + experiment_uuid.to_string()
           + " --creation_request_uuid=" + creation_request_id.to_string()
           + " --environment_manager_registration_endpoint="
           + node_config.environment_manager_registration_endpoint.to_string()
           + " --ip_address=" + node_config.control_ip_address.to_string() + " --lib_root_dir="
           + node_config.lib_root_dir + " --re_bin_dir=" + node_config.re_bin_path
           + " --registration_entity_uuid=" + node_config.uuid.to_string();
}
} // namespace detail

NodeManager::NodeManager(NodeConfig config, EpmRegistry& epm_registry) :
    epm_registry_{epm_registry},
    node_config_{std::move(config)},
    control_service_{std::make_shared<NodeManagerControlImpl>(epm_registry_)},
    server_{node_config_.control_ip_address, {control_service_}}
{
    // Test that we can find the EPM executable before doing anything else.
    // Fail early my dudes.
    detail::find_epm_executable(node_config_.re_bin_path);

    std::cout << "[NodeManager] - Started with uuid: (" << node_config_.uuid << ")" << std::endl;
    // We now have all of our threads/repliers set up so we can register with the env manager.
    // Register with environment manager. Throws on failure.
    detail::register_node_manager(node_config_, server_.endpoint());
    std::cout << "[NodeManager] - Registered with environment manager at:\n    ("
              << node_config_.environment_manager_registration_endpoint.to_string() << ")"
              << std::endl;
}

NodeManager::~NodeManager()
{
    try {
        detail::deregister_node_manager(node_config_);
    } catch(const std::exception& ex) {
        std::cout << ex.what() << std::endl;
    }
}


auto NodeManager::wait() -> void
{
    server_.wait();
}

auto NodeManager::shutdown() -> void
{
    server_.shutdown();
}

} // namespace sem::node_manager
