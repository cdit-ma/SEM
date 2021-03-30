//
// Created by mitchell on 10/3/21.
//

#include "epm_output_control.h"
using namespace sem::environment_controller;
epm_output_control::~epm_output_control() {}
auto
epm_output_control::StartListening(grpc::ServerContext* context,
                                   const EpmOutputControlNamespace::StartRequest* request,
                                   EpmOutputControlNamespace::StartResponse* response) -> grpc::Status
{
    return grpc::Status::OK;
}
grpc::Status
epm_output_control::StopListening(grpc::ServerContext* context,
                                  const EpmOutputControlNamespace::StopRequest* request,
                                  EpmOutputControlNamespace::StopResponse* response)
{
    lifetime_manager_.shutdown();
    return grpc::Status::OK;
}

epm_output_control::epm_output_control(grpc_util::ServerLifetimeManager& lifetime_manager,
                                       sem::types::Ipv4 listener_address) :
    listener_service_{}, server_{listener_address, {listener_service_}}, lifetime_manager_{lifetime_manager}
{
}

auto epm_output_control::get_listener_endpoint() const -> sem::types::SocketAddress
{
    return server_.endpoints().begin()->second;
}
