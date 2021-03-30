//
// Created by mitchell on 10/3/21.
//

#include "epm_output_listener.h"
namespace sem::environment_controller {

epm_output_listener::~epm_output_listener() {}
grpc::Status epm_output_listener::Output(grpc::ServerContext* context,
                                         const EpmOutputNamespace::MessageRequest* request,
                                         EpmOutputNamespace::MessageResponse* response)
{
    std::cout << request->output() << std::endl;
    return grpc::Status::OK;
}

} // namespace sem::environment_controller
