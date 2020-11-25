//
// Created by mitchell on 23/10/20.
//

#include "epmregistrarimpl.h"
namespace sem::node_manager::epm_registry {
using namespace sem::network::services::epm_registration;

EpmRegistrarImpl::~EpmRegistrarImpl() = default;

auto EpmRegistrarImpl::RegisterEpm(grpc::ServerContext* context,
                                   const RegistrationRequest* request,
                                   RegistrationResponse* response) -> grpc::Status
{
    std::lock_guard lock(registration_mutex_);
    auto container_uuid = types::Uuid{request->container_uuid()};
    if(registration_promises_.count(container_uuid)) {
        registration_promises_.at(container_uuid)
            .set_value({{request->epm_uuid()},
                        EpmInfo{types::SocketAddress{request->epm_control_endpoint()},
                                             types::Ipv4{request->data_ip_address()}}});
        registration_promises_.erase(container_uuid);
    }
    return grpc::Status::OK;
}
auto EpmRegistrarImpl::DeregisterEpm(grpc::ServerContext* context,
                                     const DeregistrationRequest* request,
                                     DeregistrationResponse* response) -> grpc::Status
{

    std::lock_guard lock(deregistration_mutex_);
    auto epm_uuid = types::Uuid{request->epm_uuid()};
    if(deregistration_promises_.count(epm_uuid)) {
        deregistration_promises_.at(epm_uuid).set_value(epm_uuid);
        deregistration_promises_.erase(epm_uuid);
    }
    return grpc::Status::OK;
}
auto EpmRegistrarImpl::wait_on_epm_registration(const EpmStartArguments& args,
                                                const types::SocketAddress& server_endpoint,
                                                const std::chrono::seconds timeout)
    -> EpmRegistrationResult
{
    using EpmRegistrationResult = EpmRegistrationResult;
    // Create grpc service with std future to fill as arg.
    std::promise<EpmRegistrationResult> promise;
    std::future<EpmRegistrationResult> future = promise.get_future();
    {
        std::lock_guard lock(registration_mutex_);
        registration_promises_.emplace(args.container_uuid, std::move(promise));
    }

    // Start epm process with args: registration server endpoint, container_uuid
    namespace bp = boost::process;

    auto start_command = args.to_command_line_args();
    start_command += " --process_registration_endpoint=" + server_endpoint.to_string();
    auto epm_process_handle = std::make_unique<bp::child>(start_command);

    // Wait for our epm to send its registration message back to the epm_registrar. Return its
    // config info s.t. the environment manager knows where to find it.
    if(future.wait_for(timeout) == std::future_status::timeout) {
        // Throw if our wait times out
        throw std::runtime_error("[EpmRegistry] - Timed out while waiting for EPM to register.");
    }

    auto registration_result = future.get();
    registration_result.process_handle = std::move(epm_process_handle);
    return registration_result;
}
EpmRegistrarImpl::EpmRegistrarImpl() = default;

} // namespace sem::node_manager
