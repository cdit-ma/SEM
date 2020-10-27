//
// Created by mitchell on 23/10/20.
//

#ifndef SEM_EPMREGISTRARIMPL_H
#define SEM_EPMREGISTRARIMPL_H

#include "epm_registration_service.grpc.pb.h"
#include "epmregistry.h"
#include <condition_variable>

namespace sem::node_manager {

class EpmRegistrarImpl final
    : public sem::network::services::epm_registration::EpmRegistrar::Service {
public:
    struct EpmEndpoints {
        types::SocketAddress control_endpoint;
        types::SocketAddress data_endpoint;
    };
    EpmRegistrarImpl();
    ~EpmRegistrarImpl() final;

    template<typename EpmStartFunction>
    auto start_epm(EpmStartFunction epm_start_function) -> EpmEndpoints
    {
        // Run boost::start child process, pass registrar address as arg.
        epm_start_function();

        std::unique_lock lock(mutex_);

        // Wait till we've received a registration call.
        if(cv_.wait_for(lock, start_epm_timeout_, [this]() { return registered_; })) {
            return *endpoints_;
        }
        throw std::runtime_error("[EpmRegistrar] - Epm failed to register within "
                                 + std::to_string(start_epm_timeout_.count()) + " seconds");
    }

    auto RegisterEpm(grpc::ServerContext* context,
                     const network::services::epm_registration::RegistrationRequest* request,
                     network::services::epm_registration::RegistrationResponse* response)
        -> grpc::Status final;
    auto DeregisterEpm(grpc::ServerContext* context,
                       const network::services::epm_registration::DeregistrationRequest* request,
                       network::services::epm_registration::DeregistrationResponse* response)
        -> grpc::Status final;

private:
    static constexpr std::chrono::seconds start_epm_timeout_{5};
    std::mutex mutex_;
    std::condition_variable cv_;
    bool registered_;
    std::optional<EpmEndpoints> endpoints_;
};

} // namespace sem::node_manager
#endif // SEM_EPMREGISTRARIMPL_H
