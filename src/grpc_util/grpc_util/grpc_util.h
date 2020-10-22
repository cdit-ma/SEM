#ifndef SEM_GRPC_UTIL_H
#define SEM_GRPC_UTIL_H

#include "socketaddress.hpp"
#include <grpcpp/create_channel.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <memory>
#include <utility>
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
    -> std::pair<sem::types::SocketAddress, std::unique_ptr<grpc::Server>>;

template<typename Type>
auto get_stub(const sem::types::SocketAddress& stub_endpoint) -> std::unique_ptr<typename Type::Stub>
{
    return Type::NewStub(
        grpc::CreateChannel(stub_endpoint.to_string(), grpc::InsecureChannelCredentials()));
};
} // namespace sem::grpc_util

#endif // SEM_GRPC_UTIL_H
