//
// Created by mitchell on 21/10/20.
//

#ifndef SEM_SERVER_H
#define SEM_SERVER_H

#include "socketaddress.hpp"
#include <grpcpp/create_channel.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <optional>

namespace sem::grpc_util {

class Server {
public:
    using GrpcServiceVector = std::vector<std::weak_ptr<grpc::Service>>;
    /**
     *
     * Don't use this api unless you know what you're doing.
     * Prefer Server(grpc::Service&, types::Ipv4) by default
     */
    Server(types::SocketAddress addr, const GrpcServiceVector& services);

    /**
     * Starts a grpc server with given service on given ip addr. Selects random free port.
     * To get socket address server was started on, use .address()
     */
    Server(types::Ipv4 addr, const GrpcServiceVector& services);

    [[nodiscard]] auto endpoint() const -> types::SocketAddress;
    auto wait() const -> void;
    auto shutdown() const -> void;

private:
    std::unique_ptr<grpc::Server> server_;
    // We use optional here for delayed initialisation
    std::optional<types::SocketAddress> endpoint_;
};

} // namespace sem::grpc_util
#endif // SEM_SERVER_H
