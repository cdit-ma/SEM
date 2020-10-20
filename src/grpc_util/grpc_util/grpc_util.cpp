//
// Created by mitchell on 20/10/20.
//

#include "grpc_util.h"

namespace sem::grpc_util {
/**
 * Run a grpc server with the given service on the given endpoint
 * @param bind_address Socket addr to run the server on. Set port to 0 for
 * random port
 * @param service Service to run
 * @return Pair containing the endpoint the server is running on and a pointer
 * to the server.
 */
auto run_grpc_server(const sem::types::SocketAddress& bind_address, grpc::Service& service)
    -> std::pair<sem::types::SocketAddress, std::unique_ptr<grpc::Server>>
{
    // TODO: Should investigate using enable_if to narrow template resolution
    grpc::ServerBuilder builder;
    int assigned_port{};

    builder.AddListeningPort(bind_address.to_string(), grpc::InsecureServerCredentials(),
                             &assigned_port);
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server_ptr(builder.BuildAndStart());

    return {sem::types::SocketAddress(bind_address.ip(), assigned_port), std::move(server_ptr)};
}
} // namespace sem::grpc_util