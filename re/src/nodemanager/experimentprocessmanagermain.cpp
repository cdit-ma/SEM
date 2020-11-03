#include "epm_registration_service.grpc.pb.h"
#include "experimentprocessmanagerconfig.h"
#include <exception>
#include <grpc_util/grpc_util.h>
#include <iostream>
auto main(int argc, char** argv) -> int
{
    using namespace sem::experiment_process_manager;
    using namespace sem::network::services::epm_registration;

    try {
        auto config = ExperimentProcessManagerConfig::ParseArguments(argc, argv);

        auto registration_stub =
            sem::grpc_util::get_stub<EpmRegistrar>(
                config.process_registration_endpoint);

        RegistrationRequest request;
        request.set_uuid(config.epm_uuid.to_string());
        request.set_data_ip_address(config.data_ip_address.to_string());


        request.set_epm_control_endpoint(config.process_registration_endpoint.to_string());

        RegistrationResponse response;
        grpc::ClientContext context;
        auto status = registration_stub->RegisterEpm(&context, request, &response);
        if(!status.ok()) {
            std::cout << config.process_registration_endpoint.to_string() << std::endl;
            std::cout << "req failed" << std::endl;
        }
        std::cout << "sent req" << std::endl;

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
