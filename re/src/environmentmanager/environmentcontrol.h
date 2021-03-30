//
// Created by mitchell on 17/2/21.
//

#ifndef SEM_ENVIRONMENTCONTROL_H
#define SEM_ENVIRONMENTCONTROL_H
#include "environment_control_service.grpc.pb.h"
#include "nodemanagerregistryimpl.h"

namespace sem::environment_manager {

namespace ServiceNamespace = sem::network::services::environment_manager;
class EnvironmentControl final : public ServiceNamespace::EnvironmentControl::Service {
public:
    explicit EnvironmentControl(NodeManagerRegistry& node_manager_registry);
    auto AddExperiment(grpc::ServerContext* context,
                       const ServiceNamespace::AddExperimentRequest* request,
                       ServiceNamespace::AddExperimentResponse* response) -> grpc::Status final;
    auto StartExperiment(grpc::ServerContext* context,
                         const ServiceNamespace::StartExperimentRequest* request,
                         ServiceNamespace::StartExperimentResponse* response) -> grpc::Status final;
    auto ShutdownExperiment(grpc::ServerContext* context,
                            const ServiceNamespace::ShutdownExperimentRequest* request,
                            ServiceNamespace::ShutdownExperimentResponse* response)
        -> grpc::Status final;

    auto
    InspectExperiment(grpc::ServerContext* context,
                      const ServiceNamespace::InspectExperimentRequest* request,
                      ServiceNamespace::InspectExperimentResponse* response) -> grpc::Status final;

    auto ListExperiments(grpc::ServerContext* context,
                         const ServiceNamespace::ListExperimentsRequest* request,
                         ServiceNamespace::ListExperimentsResponse* response) -> grpc::Status final;

private:
    NodeManagerRegistry& nm_registry_;
};

} // namespace sem::environment_manager
#endif // SEM_ENVIRONMENTCONTROL_H
