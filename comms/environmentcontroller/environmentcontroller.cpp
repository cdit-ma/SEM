#include "environmentcontroller.h"
#include <proto/environmentcontrol/environmentcontrol.pb.h>

EnvironmentManager::EnvironmentController::EnvironmentController(const std::string& environment_manager_endpoint):
    requester_(environment_manager_endpoint)
{
}

void EnvironmentManager::EnvironmentController::ShutdownExperiment(const std::string& experiment_name){
    using namespace EnvironmentControl;
    ShutdownExperimentRequest request;
    request.set_experiment_name(experiment_name);

    auto reply = requester_.SendRequest<ShutdownExperimentRequest, ShutdownExperimentReply>("ShutdownExperiment", request, 100);
    try{
        reply.get();
    }catch(const zmq::RMIException& ex){
        throw std::invalid_argument(ex.what());
    }catch(const zmq::TimeoutException& ex){
        throw std::runtime_error(ex.what());
    }
}

std::vector<std::string> EnvironmentManager::EnvironmentController::ListExperiments(){
    using namespace EnvironmentControl;
    ListExperimentsRequest request;
    auto reply = requester_.SendRequest<ListExperimentsRequest, ListExperimentsReply>("ListExperiments", request, 100);
    try{
        auto experiment_pb = reply.get();
        const auto& experiment_names = experiment_pb->experiment_names();
        return {experiment_names.begin(), experiment_names.end()};
    }catch(const zmq::RMIException& ex){
        throw std::invalid_argument(ex.what());
    }catch(const zmq::TimeoutException& ex){
        throw std::runtime_error(ex.what());
    }
}
