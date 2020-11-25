#ifndef SEM_NODEMANAGERREGISTRARIMPL_H
#define SEM_NODEMANAGERREGISTRARIMPL_H

#include "ipv4.hpp"
#include "node_manager_registration_service.grpc.pb.h"
#include "nodemanagerregistryimpl.h"
#include "nodemanagerregistryprovider.h"
#include "socketaddress.hpp"
#include "uuid.h"
namespace sem::environment_manager {
namespace ServiceNamespace = sem::network::services::environment_manager;

class NodeManagerRegistrarImpl final : public ServiceNamespace::NodeManagerRegistrar::Service {
public:
    explicit NodeManagerRegistrarImpl(NodeManagerRegistryProvider& registry_provider);
    explicit NodeManagerRegistrarImpl(NodeManagerRegistry& registry);
    ~NodeManagerRegistrarImpl() final;
    auto
    RegisterNodeManager(grpc::ServerContext* context,
                        const ServiceNamespace::RegistrationRequest* request,
                        ServiceNamespace::RegistrationResponse* response) -> grpc::Status final;
    auto
    DeregisterNodeManager(grpc::ServerContext* context,
                          const ServiceNamespace::DeregistrationRequest* request,
                          ServiceNamespace::DeregistrationResponse* response) -> grpc::Status final;

private:
    NodeManagerRegistry& registry_;
};
} // namespace sem::environment_manager

#endif // SEM_NODEMANAGERREGISTRARIMPL_H
