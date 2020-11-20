#ifndef SEM_GRPC_UTIL_H
#define SEM_GRPC_UTIL_H

#include "socketaddress.hpp"
#include <grpcpp/create_channel.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <memory>
#include <utility>
namespace sem::grpc_util {
template<typename Type>
auto get_stub(const sem::types::SocketAddress& stub_endpoint) -> std::unique_ptr<typename Type::Stub>
{
    return Type::NewStub(
        grpc::CreateChannel(stub_endpoint.to_string(), grpc::InsecureChannelCredentials()));
};
} // namespace sem::grpc_util

#endif // SEM_GRPC_UTIL_H
