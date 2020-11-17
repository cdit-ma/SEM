#ifndef SEM_NODEMANAGERCONTROLIMPL_H
#define SEM_NODEMANAGERCONTROLIMPL_H
#include "epmregistry.h"
#include "node_manager_control_service.grpc.pb.h"

namespace sem::node_manager {

/**
 * This class is NOT thread safe.
 *  The underlying epm_registry_ SHOULD be thread safe.
 */

class NodeManagerControlImpl final
    : public sem::network::services::node_manager_control::NodeManagerControl::Service {
public:
    NodeManagerControlImpl(EpmRegistry& epm_registry);
    ~NodeManagerControlImpl() final;
    auto NewEpm(grpc::ServerContext* context,
                const sem::network::services::node_manager_control::NewEpmRequest* request,
                sem::network::services::node_manager_control::NewEpmResponse* response)
        -> grpc::Status final;
    auto StopEpm(grpc::ServerContext* context,
                 const sem::network::services::node_manager_control::StopEpmRequest* request,
                 sem::network::services::node_manager_control::StopEpmResponse* response)
        -> grpc::Status final;

private:
    EpmRegistry& epm_registry_;
};
} // namespace sem::node_manager

#endif // SEM_NODEMANAGERCONTROLIMPL_H
