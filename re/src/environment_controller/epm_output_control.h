//
// Created by mitchell on 10/3/21.
//

#ifndef SEM_EPM_OUTPUT_CONTROL_H
#define SEM_EPM_OUTPUT_CONTROL_H

#include "epm_output_control_service.grpc.pb.h"
#include "epm_output_listener.h"
#include <grpc_util/server.h>

namespace sem::environment_controller {

namespace EpmOutputControlNamespace = sem::network::services::epm_output_control_service;
class epm_output_control
    : public EpmOutputControlNamespace::Control::Service {

public:
    epm_output_control(grpc_util::ServerLifetimeManager& lifetime_manager, types::Ipv4 listener_address);
    ~epm_output_control() override;
    grpc::Status StartListening(grpc::ServerContext* context,
                                const EpmOutputControlNamespace::StartRequest* request,
                                EpmOutputControlNamespace::StartResponse* response) override;
    grpc::Status StopListening(grpc::ServerContext* context,
                               const EpmOutputControlNamespace::StopRequest* request,
                               EpmOutputControlNamespace::StopResponse* response) override;
    auto get_listener_endpoint() const -> sem::types::SocketAddress;

private:
    std::shared_ptr<epm_output_listener> listener_service_;

    sem::grpc_util::Server server_;

    grpc_util::ServerLifetimeManager& lifetime_manager_;
};

} // namespace sem::environment_controller
#endif // SEM_EPM_OUTPUT_CONTROL_H
