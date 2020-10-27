//
// Created by mitchell on 23/10/20.
//

#include "epmregistrarimpl.h"
namespace sem::node_manager {
using namespace sem::network::services::epm_registration;

EpmRegistrarImpl::EpmRegistrarImpl() {}
EpmRegistrarImpl::~EpmRegistrarImpl() = default;

auto EpmRegistrarImpl::RegisterEpm(grpc::ServerContext* context,
                                   const RegistrationRequest* request,
                                   RegistrationResponse* response) -> grpc::Status
{
    // Set config details, control_endpoint, data_endpoint

    // Notify waiting "start_epm" call.
    {
        std::lock_guard lock(mutex_);
        registered_ = true;
    }
    cv_.notify_all();
}
auto EpmRegistrarImpl::DeregisterEpm(grpc::ServerContext* context,
                                     const DeregistrationRequest* request,
                                     DeregistrationResponse* response) -> grpc::Status
{
    return grpc::Status::OK;
}




} // namespace sem::node_manager
