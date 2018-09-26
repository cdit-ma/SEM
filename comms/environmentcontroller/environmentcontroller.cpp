#include "environmentcontroller.h"
#include <proto/environmentcontrol/environmentcontrol.pb.h>
#include <util/graphmlparser/protobufmodelparser.h>

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

std::unique_ptr<NodeManager::RegisterExperimentReply> EnvironmentManager::EnvironmentController::AddExperiment(const std::string& experiment_name, const std::string& graphml_path){
    using namespace NodeManager;

    RegisterExperimentRequest request;

    request.mutable_id()->set_experiment_name(experiment_name);
    
    try{
        ProtobufModelParser parser(graphml_path, experiment_name);
        auto control_message = parser.ControlMessage();
        if(control_message){
            request.set_allocated_control_message(control_message);
        }
    }catch(const std::exception& ex){
        throw;
    }



    auto reply = requester_.SendRequest<RegisterExperimentRequest, RegisterExperimentReply>("RegisterExperiment", request, 5000);
    try{
        return std::move(reply.get());
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
