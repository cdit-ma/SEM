#include "epm_registration_service.grpc.pb.h"
#include "experimentprocessmanagerconfig.h"
#include <exception>
#include <grpc_util/grpc_util.h>
#include <iostream>
#include <thread>

bool running = true;
std::function<void(void)> interrupt_function;

void signal_handler(int sig)
{
    if(!interrupt_function) {
        std::cerr << "[EpmMain] - NO VALID INTERRUPT HANDLER SET" << std::endl;
        assert(false);
    }
    interrupt_function();
}

void register_process(const sem::experiment_process_manager::ExperimentProcessManagerConfig& config,
                      sem::types::SocketAddress epm_control_endpoint)
{
    using namespace sem::network::services::epm_registration;
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
    const sem::experiment_process_manager::ExperimentProcessManagerConfig& config)
{
    // TODO: Implement!
    std::cout << "Deregistered process" << std::endl;
    running = false;
}

auto main(int argc, char** argv) -> int
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    using namespace sem::experiment_process_manager;

    try {
        auto config = ExperimentProcessManagerConfig::ParseArguments(argc, argv);
        interrupt_function = [&config]() { deregister_process(config); };

        auto epm_control_endpoint = sem::types::SocketAddress::unspecified();

        register_process(config, epm_control_endpoint);


        // START epm_control_service and server.


        // Add epm control service's shutdown


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
