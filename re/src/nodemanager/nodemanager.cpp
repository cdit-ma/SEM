#include "nodemanager.h"
#include "grpc_util/grpc_util.h"
#include "node_manager_registration_service.grpc.pb.h"
#include "nodemanagerconfig.h"
#include <fstream>
#include <utility>

namespace sem::node_manager {

using namespace sem::network::services::node_manager_registration;
/// Tries to register this NodeManager with the EnvironmentManager.
/// Throws on any sort of registration failure.
auto register_node_manager(NodeManagerRegistrar::StubInterface& registration_stub,
                           const NodeConfig& config,
                           sem::types::SocketAddress control_endpoint) -> void
{
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
    auto status = registration_stub.RegisterNodeManager(&context, request, &reply);
    if(!status.ok()) {
        throw std::runtime_error("[NodeManager] - Failed to connect to environment manager. gRPC "
                                 "call cancelled.");
    }
    if(!reply.success()) {
        throw std::runtime_error("[NodeManager] - Failed to connect to environment manager.\n    "
                                 + reply.message());
    }
}

auto deregister_node_manager(NodeManagerRegistrar::StubInterface& registration_stub,
                             const NodeConfig& config) -> void
{
    DeregistrationRequest request;
    request.set_uuid(config.uuid.to_string());

    DeregistrationResponse reply;
    grpc::ClientContext context;
    auto status = registration_stub.DeregisterNodeManager(&context, request, &reply);
    if(!status.ok()) {
        throw std::runtime_error("[NodeManager] - Failed to deregister node manager. gRPC "
                                 "call cancelled.");
    }
    if(!reply.success()) {
        throw std::runtime_error("[NodeManager] - Failed to deregister node manager.\n    "
                                 + reply.message());
    }
}

NodeManager::NodeManager(NodeConfig config, EpmRegistry& epm_registry) :
    epm_registry_{epm_registry},
    node_config_{std::move(config)},
    control_service_{std::make_shared<NodeManagerControlImpl>(epm_registry_)},
    server_{node_config_.control_ip_address, {control_service_}}
{
    std::cout << "[NodeManager] - Started with uuid: (" << node_config_.uuid << ")" << std::endl;
    // We now have all of our threads/repliers set up so we can register with the env manager.
    // Register with environment manager. Throws on failure.
    auto registration_stub = sem::grpc_util::get_stub<NodeManagerRegistrar>(
        config.environment_manager_registration_endpoint);
    register_node_manager(*registration_stub, node_config_, server_.endpoint());
    std::cout << "[NodeManager] - Registered with environment manager at:\n    ("
              << node_config_.environment_manager_registration_endpoint.to_string() << ")"
              << std::endl;

    try {
        auto epm_uuid = epm_registry.start_epm(types::Uuid{}, types::Uuid{});
    } catch(const std::exception& ex) {
        // deregister if anything goes wrong
        deregister_node_manager(*registration_stub, node_config_);
        throw;
    }
}

NodeManager::~NodeManager()
{
    try {
        auto registration_stub = sem::grpc_util::get_stub<NodeManagerRegistrar>(
            node_config_.environment_manager_registration_endpoint);
        deregister_node_manager(*registration_stub, node_config_);
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
