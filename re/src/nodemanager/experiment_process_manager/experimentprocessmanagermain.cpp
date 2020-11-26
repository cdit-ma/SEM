#include "epm_registration_service.grpc.pb.h"
#include "experimentprocessmanagerconfig.h"
#include "grpc_util/grpc_util.h"
#include "grpc_util/server.h"
#include "grpc_util/serverlifetimemanagerimpl.h"
#include <exception>
#include <iostream>

namespace sem::experiment_process_manager::main {

bool running = true;
std::optional<grpc_util::ServerLifetimeManagerImpl> lifetime_manager;
std::optional<ExperimentProcessManagerConfig> epm_config;

void register_process(const ExperimentProcessManagerConfig& config,
                      types::SocketAddress epm_control_endpoint)
{
    using namespace sem::network::services::node_manager;
    RegistrationRequest request;
    request.set_epm_uuid(config.epm_uuid.to_string());
    request.set_container_uuid(config.container_uuid.to_string());
    request.set_data_ip_address(config.data_ip_address.to_string());

    request.set_epm_control_endpoint(epm_control_endpoint.to_string());

    RegistrationResponse response;
    grpc::ClientContext context;

    auto registration_stub = sem::grpc_util::get_stub<EpmRegistrar>(
        config.process_registration_endpoint);

    auto status = registration_stub->RegisterEpm(&context, request, &response);
    if(!status.ok()) {
        throw std::runtime_error("[EpmMain] - Failed to register with node manager.");
    }
}

void deregister_process(
    const ExperimentProcessManagerConfig& config)
{
    using namespace sem::network::services::node_manager;
    DeregistrationRequest request;
    request.set_epm_uuid(config.epm_uuid.to_string());

    DeregistrationResponse response;
    grpc::ClientContext context;

    auto registration_stub = sem::grpc_util::get_stub<EpmRegistrar>(
        config.process_registration_endpoint);

    auto status = registration_stub->DeregisterEpm(&context, request, &response);
    if(!status.ok()) {
        throw std::runtime_error("[EpmMain] - Failed to deregister with node manager.");
    }
    std::cout << "Deregistered process" << std::endl;
    running = false;
}

} // namespace sem::experiment_process_manager::main
void signal_handler(int sig)
{
    using namespace sem::experiment_process_manager::main;

    if(lifetime_manager) {
        lifetime_manager->shutdown();
    } else {
        std::cerr << "[EPMMain] - signal_handler called before lifetime_manager initialised."
                  << std::endl;
    }
    if(epm_config) {
        deregister_process(*epm_config);
    } else {
        std::cerr << "[EPMMain] - signal_handler called before config initialised."
                  << std::endl;
    }
}

auto main(int argc, char** argv) -> int
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    try {
        using namespace sem::experiment_process_manager;
        using namespace sem::experiment_process_manager::main;
        epm_config = ExperimentProcessManagerConfig::ParseArguments(argc, argv);

        // START epm_control_service and server.
        lifetime_manager.emplace();
        sem::grpc_util::LifetimeManagedServer server(*lifetime_manager, epm_config->control_ip_address,
                                                     {/*services*/});

        register_process(*epm_config, server.endpoints().at(epm_config->control_ip_address));
        lifetime_manager->wait();
        deregister_process(*epm_config);

    } catch(const std::invalid_argument& ex) {
        // Return non-zero on failure
        return 1;
    } catch(const std::exception& ex) {
        std::cout << ex.what() << std::endl;
        // Return non-zero on failure
        return 1;
    }
    return 0;
}
