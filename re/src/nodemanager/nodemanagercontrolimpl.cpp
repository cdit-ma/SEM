//
// Created by mitchell on 22/10/20.
//

#include "nodemanagercontrolimpl.h"

namespace sem::node_manager {
using namespace network::services::node_manager_control;
NodeManagerControlImpl::~NodeManagerControlImpl() = default;

grpc::Status NodeManagerControlImpl::NewEpm(grpc::ServerContext* context,
                                            const NewEpmRequest* request,
                                            NewEpmResponse* response)
{
    epm_registry_.start_epm();
    return Service::NewEpm(context, request, response);
}

grpc::Status NodeManagerControlImpl::StopEpm(grpc::ServerContext* context,
                                             const StopEpmRequest* request,
                                             StopEpmResponse* response)
{
    return Service::StopEpm(context, request, response);
}
NodeManagerControlImpl::NodeManagerControlImpl(EpmRegistry& epm_registry) :
    epm_registry_{epm_registry}
{
}

} // namespace sem::node_manager
