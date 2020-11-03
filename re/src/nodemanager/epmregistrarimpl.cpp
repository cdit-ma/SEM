//
// Created by mitchell on 23/10/20.
//

#include "epmregistrarimpl.h"
namespace sem::node_manager {
using namespace sem::network::services::epm_registration;

EpmRegistrarImpl::~EpmRegistrarImpl() = default;

auto EpmRegistrarImpl::RegisterEpm(grpc::ServerContext* context,
                                   const RegistrationRequest* request,
                                   RegistrationResponse* response) -> grpc::Status
{
    promise_to_fill_.set_value(
        {{request->uuid()},
         EpmRegistry::EpmInfo{types::SocketAddress{request->epm_control_endpoint()},
                               types::Ipv4{request->data_ip_address()}}});
    return grpc::Status::OK;
}
auto EpmRegistrarImpl::DeregisterEpm(grpc::ServerContext* context,
                                     const DeregistrationRequest* request,
                                     DeregistrationResponse* response) -> grpc::Status
{
    return grpc::Status::OK;
}
EpmRegistrarImpl::EpmRegistrarImpl(std::promise<EpmRegistrationResult> promise_to_fill) :
    promise_to_fill_{std::move(promise_to_fill)}
{
}

} // namespace sem::node_manager
