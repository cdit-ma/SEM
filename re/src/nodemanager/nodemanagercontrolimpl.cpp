#include "nodemanagercontrolimpl.h"

namespace sem::node_manager {
using namespace network::services::node_manager;
NodeManagerControlImpl::~NodeManagerControlImpl() = default;

grpc::Status NodeManagerControlImpl::NewEpm(grpc::ServerContext* context,
                                            const NewEpmRequest* request,
                                            NewEpmResponse* response)
{
    try {
        // TODO: Add experiment registration endpoint as param here
        auto epm_uuid = epm_registry_.start_epm({request->experiment_uuid()}, {request->container_uuid()});
        auto epm_info = epm_registry_.get_epm_info(epm_uuid);
        response->set_success(true);
        response->set_epm_uuid(epm_uuid.to_string());
        response->set_epm_control_endpoint(epm_info.control_endpoint.to_string());
        response->set_epm_data_ip_address(epm_info.data_ip_address.to_string());
        return grpc::Status::OK;
    } catch (const std::exception& ex) {
        response->set_success(false);
        response->set_error_message(ex.what());
        return grpc::Status::OK;
    }
}

grpc::Status NodeManagerControlImpl::StopEpm(grpc::ServerContext* context,
                                             const StopEpmRequest* request,
                                             StopEpmResponse* response)
{

    epm_registry_.remove_epm(types::Uuid{request->epm_uuid()});
    return Service::StopEpm(context, request, response);
}
NodeManagerControlImpl::NodeManagerControlImpl(epm_registry::EpmRegistry& epm_registry) :
    epm_registry_{epm_registry}
{
}

} // namespace sem::node_manager
