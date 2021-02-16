#include "nodemanagerregistrarimpl.h"
namespace sem::environment_manager {
using namespace network::services::environment_manager;

NodeManagerRegistrarImpl::NodeManagerRegistrarImpl(NodeManagerRegistryProvider& registry_provider) :
    NodeManagerRegistrarImpl(registry_provider.get_registry())
{
}
NodeManagerRegistrarImpl::NodeManagerRegistrarImpl(NodeManagerRegistry& registry) :
    registry_(registry)
{
}
NodeManagerRegistrarImpl::~NodeManagerRegistrarImpl() = default;

auto NodeManagerRegistrarImpl::RegisterNodeManager(grpc::ServerContext* context,
                                                   const RegistrationRequest* request,
                                                   RegistrationResponse* response) -> grpc::Status
{
    try {
        registry_.add_node_manager(types::Uuid(request->uuid()),
                                   {types::Ipv4(request->control_ip_address()),
                                    types::Ipv4(request->data_ip_address()),
                                    types::SocketAddress(request->node_manager_control_endpoint()),
                                    request->hostname(), request->library_path()});
        response->set_success(true);
    } catch(const std::exception& ex) {
        response->set_success(false);
        response->set_error_message(ex.what());
    }
    return grpc::Status::OK;
}

auto NodeManagerRegistrarImpl::DeregisterNodeManager(grpc::ServerContext* context,
                                                     const DeregistrationRequest* request,
                                                     DeregistrationResponse* response)
    -> grpc::Status
{
    try {
        registry_.remove_node_manager(types::Uuid{request->uuid()});
        response->set_success(true);
    } catch(const std::exception& ex) {
        response->set_success(false);
        response->set_error_message(ex.what());
    }
    return grpc::Status::OK;
}

} // namespace sem::environment_manager