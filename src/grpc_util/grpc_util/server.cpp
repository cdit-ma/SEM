//
// Created by mitchell on 21/10/20.
//

#include "server.h"

namespace sem::grpc_util {
namespace detail {
auto run_grpc_server(const types::SocketAddress& bind_address,
                     const Server::GrpcServiceVector& services)
    -> std::pair<types::SocketAddress, std::unique_ptr<grpc::Server>>
{
    grpc::ServerBuilder builder;
    int assigned_port{};

    builder.AddListeningPort(bind_address.to_string(), grpc::InsecureServerCredentials(),
                             &assigned_port);

    for(const auto& service : services) {
        builder.RegisterService(service.lock().get());
    }

    // Build and start returns a server unique ptr, this is doing elided move assignment.
    std::unique_ptr<grpc::Server> server_ptr{builder.BuildAndStart()};

    return {types::SocketAddress(bind_address.ip(), assigned_port), std::move(server_ptr)};
}
} // namespace detail

auto Server::address() const -> types::SocketAddress
{
    // Address should not actually be optional, only use optional for delayed init
    return *address_;
}
Server::Server(types::SocketAddress addr, const GrpcServiceVector& services)
{
    auto [assigned_address, server] = detail::run_grpc_server(addr, services);
    address_ = assigned_address;
    server_ = std::move(server);
}
Server::Server(types::Ipv4 addr, const GrpcServiceVector& services)
{
    auto [assigned_address, server] = detail::run_grpc_server(types::SocketAddress(addr, 0),
                                                              services);

    address_ = assigned_address;
    server_ = std::move(server);
}
auto Server::wait() const -> void
{
    server_->Wait();
}
} // namespace sem::grpc_util