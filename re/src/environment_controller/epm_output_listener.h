#ifndef SEM_EPM_OUTPUT_LISTENER_H
#define SEM_EPM_OUTPUT_LISTENER_H

#include "epm_output_service.grpc.pb.h"

namespace sem::environment_controller {

namespace EpmOutputNamespace = sem::network::services::epm_output_service;
class epm_output_listener : public EpmOutputNamespace::Listening::Service {
public:
    ~epm_output_listener() override;
    grpc::Status Output(grpc::ServerContext* context,
                        const EpmOutputNamespace::MessageRequest* request,
                        EpmOutputNamespace::MessageResponse* response) override;
};

} // namespace sem::environment_controller
#endif // SEM_EPM_OUTPUT_LISTENER_H
