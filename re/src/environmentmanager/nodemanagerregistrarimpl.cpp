#include "nodemanagerregistrarimpl.h"
namespace sem::environment_manager {
using namespace network::services::node_manager_registration;

NodeManagerRegistrarImpl::NodeManagerRegistrarImpl(NodeManagerRegistryProvider& registry_provider) :
    registry_{registry_provider.get_registry()}
{
}
NodeManagerRegistrarImpl::~NodeManagerRegistrarImpl() = default;

auto NodeManagerRegistrarImpl::RegisterNodeManager(grpc::ServerContext* context,
                                                   const RegistrationRequest* request,
                                                   RegistrationReply* response) -> grpc::Status
{
    try {
        registry_.add_node_manager(types::Uuid(request->uuid()),
                                   {types::Ipv4(request->control_ip_address()),
                                    types::Ipv4(request->data_ip_address()),
                                    types::SocketAddress(request->node_manager_control_endpoint()),
                                    request->hostname(), request->library_path()});
    } catch(const std::exception& ex) {
        response->set_success(false);
        response->set_message(ex.what());
    }
    return grpc::Status::OK;
}

auto NodeManagerRegistrarImpl::DeregisterNodeManager(grpc::ServerContext* context,
                                                     const DeregistrationRequest* request,
                                                     DeregistrationReply* response) -> grpc::Status
{
    try {
        registry_.remove_node_manager(types::Uuid{request->uuid()});
    } catch(const std::exception& ex) {
        response->set_success(false);
        response->set_message(ex.what());
    }
    return grpc::Status::OK;
}
NodeManagerRegistrarImpl::NodeManagerRegistrarImpl(NodeManagerRegistry& registry) :
    registry_(registry)
{
}
} // namespace sem::environment_manager