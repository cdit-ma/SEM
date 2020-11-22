//
// Created by mitchell on 21/10/20.
//

#include "server.h"

namespace sem::grpc_util {
namespace detail {
auto run_grpc_server(const types::SocketAddress& bind_address, const GrpcServiceVector& services)
    -> std::pair<types::SocketAddress, std::unique_ptr<grpc::Server>>
{
    grpc::ServerBuilder builder;
    // Zero here is used to signal to builder.AddListeningPort that we want a randomly assigned port
    int assigned_port{bind_address.port()};

    builder.AddListeningPort(bind_address.to_string(), grpc::InsecureServerCredentials(),
                             &assigned_port);

    for(const auto& service : services) {
        builder.RegisterService(service.lock().get());
    }

    // Build and start returns a server unique ptr, this is doing elided move assignment.
    std::unique_ptr<grpc::Server> server_ptr{builder.BuildAndStart()};

    return {types::SocketAddress(bind_address.ip(), assigned_port), std::move(server_ptr)};
}
auto run_grpc_server(const types::Ipv4& bind_address, const GrpcServiceVector& services)
    -> std::pair<types::SocketAddress, std::unique_ptr<grpc::Server>>
{
    return run_grpc_server(types::SocketAddress(bind_address, 0), services);
}
} // namespace detail

auto Server::endpoint() const -> types::SocketAddress
{
    // Address should not actually be optional, only use optional for delayed init
    return *endpoint_;
}
Server::Server(types::SocketAddress addr, const GrpcServiceVector& services)
{
    auto [assigned_address, server] = detail::run_grpc_server(addr, services);
    endpoint_ = assigned_address;
    server_ = std::move(server);
}
Server::Server(types::Ipv4 addr, const GrpcServiceVector& services)
{
    auto [assigned_address, server] = detail::run_grpc_server(addr, services);

    endpoint_ = assigned_address;
    server_ = std::move(server);
}
auto Server::wait() const -> void
{
    server_->Wait();
}
auto Server::shutdown() const -> void
{
    server_->Shutdown();
}

LifetimeManagedServer::LifetimeManagedServer(ServerLifetimeManager& lifetime_manager,
                                             types::Ipv4 addr,
                                             const GrpcServiceVector& services) :
    lifetime_manager_{lifetime_manager}
{
    auto [assigned_address, server] = detail::run_grpc_server(types::SocketAddress(addr, 0),
                                                              services);
    endpoint_ = assigned_address;
    server_ = std::move(server);
    lifetime_manager_.set_shutdown_call([this]() { server_->Shutdown(); });
    lifetime_manager_.set_wait_call([this]() { server_->Wait(); });
}

LifetimeManagedServer::LifetimeManagedServer(ServerLifetimeManager& lifetime_manager,
                                             types::SocketAddress addr,
                                             const GrpcServiceVector& services) :
    lifetime_manager_{lifetime_manager}
{
    auto [assigned_address, server] = detail::run_grpc_server(addr, services);
    endpoint_ = assigned_address;
    server_ = std::move(server);
    lifetime_manager_.set_shutdown_call([this]() { server_->Shutdown(); });
    lifetime_manager_.set_wait_call([this]() { server_->Wait(); });
}
} // namespace sem::grpc_util