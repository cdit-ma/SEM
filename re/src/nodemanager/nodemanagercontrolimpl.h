//
// Created by mitchell on 22/10/20.
//

#ifndef SEM_NODEMANAGERCONTROLIMPL_H
#define SEM_NODEMANAGERCONTROLIMPL_H
#include "epmregistry.h"
#include "node_manager_control_service.grpc.pb.h"

namespace sem::node_manager {

class NodeManagerControlImpl
    : public sem::network::services::node_manager_control::NodeManagerControl::Service {
public:
    NodeManagerControlImpl(EpmRegistry& epm_registry);
    ~NodeManagerControlImpl() override;
    grpc::Status
    NewEpm(grpc::ServerContext* context,
           const sem::network::services::node_manager_control::NewEpmRequest* request,
           sem::network::services::node_manager_control::NewEpmResponse* response) override;
    grpc::Status
    StopEpm(grpc::ServerContext* context,
            const sem::network::services::node_manager_control::StopEpmRequest* request,
            sem::network::services::node_manager_control::StopEpmResponse* response) override;

private:
    EpmRegistry& epm_registry_;
};
} // namespace sem::node_manager

#endif // SEM_NODEMANAGERCONTROLIMPL_H
