//
// Created by mitchell on 23/10/20.
//

#ifndef SEM_EPMREGISTRARIMPL_H
#define SEM_EPMREGISTRARIMPL_H

#include "epm_registration_service.grpc.pb.h"
#include "epmregistry.h"
#include <future>

namespace sem::node_manager {

class EpmRegistrarImpl final
    : public sem::network::services::epm_registration::EpmRegistrar::Service {
public:
    EpmRegistrarImpl();
    ~EpmRegistrarImpl() final;

    constexpr static std::chrono::seconds default_registration_timeout{5};
    auto wait_on_epm_registration(const EpmRegistry::EpmStartArguments& args,
                                  const types::SocketAddress& server_endpoint,
                                  std::chrono::seconds timeout = default_registration_timeout)
        -> EpmRegistry::EpmRegistrationResult;

    auto RegisterEpm(grpc::ServerContext* context,
                     const network::services::epm_registration::RegistrationRequest* request,
                     network::services::epm_registration::RegistrationResponse* response)
        -> grpc::Status final;
    auto DeregisterEpm(grpc::ServerContext* context,
                       const network::services::epm_registration::DeregistrationRequest* request,
                       network::services::epm_registration::DeregistrationResponse* response)
        -> grpc::Status final;

private:
    std::mutex registration_mutex_;
    std::unordered_map<types::Uuid, std::promise<EpmRegistry::EpmRegistrationResult>>
        registration_promises_;

    std::mutex deregistration_mutex_;
    std::unordered_map<types::Uuid, std::promise<types::Uuid>> deregistration_promises_;
};

} // namespace sem::node_manager
#endif // SEM_EPMREGISTRARIMPL_H
