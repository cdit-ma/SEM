//
// Created by mitchell on 21/10/20.
//

#ifndef SEM_SERVER_H
#define SEM_SERVER_H

#include "serverlifetimemanager.h"
#include "socketaddress.hpp"
#include <grpcpp/create_channel.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <set>
#include <unordered_map>
#include <variant>

namespace sem::grpc_util {

using GrpcServiceVector = std::vector<std::weak_ptr<grpc::Service>>;
using AddressVariant = std::variant<types::Ipv4, types::SocketAddress>;

// When we create a set of AddressVariants in this context, we want to be ensuring uniquity based on
// the Ipv4 section of both Ipv4 and SocketAddress
struct AddressVariantComparator {
    auto operator()(const AddressVariant& a, const AddressVariant& b) const -> bool
    {
        types::Ipv4 a_ip = types::Ipv4::unspecified();
        types::Ipv4 b_ip = types::Ipv4::unspecified();

        if(std::holds_alternative<types::Ipv4>(a)) {
            a_ip = std::get<types::Ipv4>(a);
        } else {
            a_ip = std::get<types::SocketAddress>(a).ip();
        }

        if(std::holds_alternative<types::Ipv4>(b)) {
            b_ip = std::get<types::Ipv4>(b);
        } else {
            b_ip = std::get<types::SocketAddress>(b).ip();
        }
        return a_ip == b_ip;
    }
};
using AddressSet = std::set<AddressVariant, AddressVariantComparator>;

class Server {
public:
    /**
     * Starts a grpc server with given service on given ip addr. Selects random free port.
     * To get socket address server was started on, use .address()
     */
    /**
     * This constructor takes a SocketAddress. This implies that you want this grpc server started
     *  on a specific port. This is not the UNIXy way of doing things. Leave port assignment to the
     *  grpc implementation as it will do its best to avoid conflicts.
     * Don't use this api unless you know what you're doing.
     * Prefer Server(grpc::Service&, types::Ipv4) by default
     */
    Server(AddressVariant addr, const GrpcServiceVector& services);

    Server(const AddressSet& addrs, const GrpcServiceVector& services);

    [[nodiscard]] auto endpoints() const -> std::unordered_map<types::Ipv4, types::SocketAddress>;
    auto wait() const -> void;
    auto shutdown() const -> void;

private:
    std::unique_ptr<grpc::Server> server_;
    std::unordered_map<types::Ipv4, types::SocketAddress> endpoints_;
};

class LifetimeManagedServer {
public:
    LifetimeManagedServer(ServerLifetimeManager& lifetime_manager,
                          AddressVariant addr,
                          const GrpcServiceVector& services);

    LifetimeManagedServer(ServerLifetimeManager& lifetime_manager,
                          const AddressSet& addr,
                          const GrpcServiceVector& services);

    auto endpoints() const -> std::unordered_map<types::Ipv4, types::SocketAddress>;
    auto wait() const -> void;
    auto shutdown() const -> void;

private:
    ServerLifetimeManager& lifetime_manager_;
    std::unique_ptr<grpc::Server> server_;
    std::unordered_map<types::Ipv4, types::SocketAddress> endpoints_;
};

} // namespace sem::grpc_util
#endif // SEM_SERVER_H
