//
// Created by mitchell on 21/10/20.
//

#include "server.h"
#include <variant>

namespace sem::grpc_util {
namespace detail {

auto bind_address(grpc::ServerBuilder& server, AddressVariant addr) -> types::SocketAddress
{
    int assigned_port{};
    types::Ipv4 ip_address = types::Ipv4::unspecified();
    types::SocketAddress socket_address = types::SocketAddress::unspecified();

    // Set all our required values from our extracted SocketAddress
    if(std::holds_alternative<types::SocketAddress>(addr)) {
        auto socket_addr = std::get<types::SocketAddress>(addr);
        ip_address = socket_addr.ip();
        assigned_port = socket_addr.port();
    }

    // Set all our required values from our extracted Ipv4 address
    // If we get an ip address, set our port to 0 s.t. grpc get a random unassigned port for us
    else if(std::holds_alternative<types::Ipv4>(addr)) {
        ip_address = std::get<types::Ipv4>(addr);
        assigned_port = 0;
        socket_address = types::SocketAddress(ip_address, assigned_port);
    }

    // If we get a variant we aren't handling, throw a fit
    else {
        throw std::logic_error("Tried to evaluate AddressVariant type that isn't accepted by "
                               "bind_address. See sem/src/grpc_util/server.h");
    }

    server.AddListeningPort(socket_address.to_string(), grpc::InsecureServerCredentials(),
                            &assigned_port);
    return types::SocketAddress(ip_address, assigned_port);
}

auto run_grpc_server(const AddressSet& addrs, const GrpcServiceVector& services)
    -> std::pair<std::unordered_map<types::Ipv4, types::SocketAddress>, std::unique_ptr<grpc::Server>>
{
    grpc::ServerBuilder builder;
    std::unordered_map<types::Ipv4, types::SocketAddress> out_map;
    for(const auto& addr : addrs) {
        types::SocketAddress bound_addr = bind_address(builder, addr);
        out_map.try_emplace(bound_addr.ip(), bound_addr);
    }

    for(const auto& service : services) {
        builder.RegisterService(service.lock().get());
    }

    // Build and start returns a server unique ptr, this is doing elided move assignment.
    std::unique_ptr<grpc::Server> server_ptr{builder.BuildAndStart()};

    return {out_map, std::move(server_ptr)};
}

} // namespace detail

auto Server::endpoint() const -> types::SocketAddress
{
    // l o l ? ?
    if(endpoints_.size() == 1) {
        return endpoints_.begin()->second;
    } else {
        throw std::logic_error("Tried to call 'endpoint()' on grpc server with multiple endpoints, "
                               "try using endpoints() instead");
    }
}

auto Server::endpoints() const -> std::unordered_map<types::Ipv4, types::SocketAddress>
{
    return endpoints_;
}
Server::Server(AddressVariant addr, const GrpcServiceVector& services) :
    Server(AddressSet{addr}, services)
{
}

Server::Server(const AddressSet& addrs, const GrpcServiceVector& services)
{
    auto [assigned_addresses, server] = detail::run_grpc_server(addrs, services);
    endpoints_ = assigned_addresses;
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
                                             const AddressSet& addrs,
                                             const GrpcServiceVector& services) :
    lifetime_manager_{lifetime_manager}
{
    auto [assigned_addresses, server] = detail::run_grpc_server(addrs, services);
    endpoints_ = assigned_addresses;
    server = std::move(server);
    lifetime_manager_.set_shutdown_call([this]() { server_->Shutdown(); });
    lifetime_manager_.set_wait_call([this]() { server_->Wait(); });
}

LifetimeManagedServer::LifetimeManagedServer(ServerLifetimeManager& lifetime_manager,
                                             AddressVariant addr,
                                             const GrpcServiceVector& services) :
    LifetimeManagedServer(lifetime_manager, AddressSet{addr}, services)
{
}

auto LifetimeManagedServer::endpoint() const -> types::SocketAddress
{
    // l 0 l ? ?
    if(endpoints_.size() == 1) {
        return endpoints_.begin()->second;
    } else {
        throw std::logic_error("Tried to call 'endpoint()' on grpc server with multiple endpoints, "
                               "try using endpoints() instead");
    }
}
auto LifetimeManagedServer::endpoints() const
    -> std::unordered_map<types::Ipv4, types::SocketAddress>
{
    return endpoints_;
}
auto LifetimeManagedServer::wait() const -> void
{
    lifetime_manager_.wait();
}
auto LifetimeManagedServer::shutdown() const -> void
{
    lifetime_manager_.shutdown();
}

} // namespace sem::grpc_util