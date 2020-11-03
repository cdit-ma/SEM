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
    struct EpmRegistrationResult {
        types::Uuid epm_uuid;
        EpmRegistry::EpmInfo epm_info;
        EpmRegistry::EpmProcessHandle process_handle;
    };
    EpmRegistrarImpl(std::promise<EpmRegistrationResult> promise_to_fill);
    ~EpmRegistrarImpl() final;

    auto RegisterEpm(grpc::ServerContext* context,
                     const network::services::epm_registration::RegistrationRequest* request,
                     network::services::epm_registration::RegistrationResponse* response)
        -> grpc::Status final;
    auto DeregisterEpm(grpc::ServerContext* context,
                       const network::services::epm_registration::DeregistrationRequest* request,
                       network::services::epm_registration::DeregistrationResponse* response)
        -> grpc::Status final;

private:
    std::promise<EpmRegistrationResult> promise_to_fill_;
};



} // namespace sem::node_manager
#endif // SEM_EPMREGISTRARIMPL_H
