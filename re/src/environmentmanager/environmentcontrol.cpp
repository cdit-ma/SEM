//
// Created by mitchell on 17/2/21.
//

#include "environmentcontrol.h"
namespace sem::environment_manager {

EnvironmentControl::EnvironmentControl(NodeManagerRegistry& node_manager_registry) :
    nm_registry_{node_manager_registry}
{
}
auto EnvironmentControl::AddExperiment(grpc::ServerContext* context,
                                       const ServiceNamespace::AddExperimentRequest* request,
                                       ServiceNamespace::AddExperimentResponse* response)
    -> grpc::Status
{
    std::cout << "adding experiment" << std::endl;
    return grpc::Status::OK;
}
auto EnvironmentControl::ShutdownExperiment(
    grpc::ServerContext* context,
    const ServiceNamespace::ShutdownExperimentRequest* request,
    ServiceNamespace::ShutdownExperimentResponse* response)
    -> grpc::Status
{
    return grpc::Status();
}
auto EnvironmentControl::InspectExperiment(grpc::ServerContext* context,
                                           const ServiceNamespace::InspectExperimentRequest* request,
                                           ServiceNamespace::InspectExperimentResponse* response)
    -> grpc::Status
{
    return grpc::Status();
}
auto EnvironmentControl::ListExperiments(grpc::ServerContext* context,
                                         const ServiceNamespace::ListExperimentsRequest* request,
                                         ServiceNamespace::ListExperimentsResponse* response)
    -> grpc::Status
{
    return grpc::Status();
}
} // namespace sem::environment_manager
