#include "environmentcontroller.h"
#include <proto/environmentcontrol/environmentcontrol.pb.h>
#include <util/graphmlparser/protobufmodelparser.h>
#include <google/protobuf/util/json_util.h>

EnvironmentManager::EnvironmentController::EnvironmentController(const std::string& environment_manager_endpoint):
    requester_(environment_manager_endpoint)
{
}

std::vector<std::string> EnvironmentManager::EnvironmentController::ShutdownExperiment(const std::string& experiment_name, bool is_regex){
    using namespace EnvironmentControl;
    ShutdownExperimentRequest request;
    request.set_experiment_name(experiment_name);
    request.set_is_regex(is_regex);
    auto reply = requester_.SendRequest<ShutdownExperimentRequest, ShutdownExperimentReply>("ShutdownExperiment", request, 100);
    auto experiment_pb = reply.get();
    const auto& experiment_names = experiment_pb->experiment_names();
    return {experiment_names.begin(), experiment_names.end()};
}

std::unique_ptr<NodeManager::RegisterExperimentReply> EnvironmentManager::EnvironmentController::AddExperiment(const std::string& experiment_name, const std::string& graphml_path){
    using namespace NodeManager;

    RegisterExperimentRequest request;
    request.mutable_id()->set_experiment_name(experiment_name);
    try{
        auto experiment = ProtobufModelParser::ParseModel(graphml_path, experiment_name);
        if(experiment){
            request.set_allocated_experiment(experiment.release());
        }
    }catch(const std::exception& ex){
        std::cerr << ex.what() << std::endl;
        throw;
    }

    auto reply = requester_.SendRequest<RegisterExperimentRequest, RegisterExperimentReply>("RegisterExperiment", request, 5000);
    return reply.get();
}

std::vector<std::string> EnvironmentManager::EnvironmentController::ListExperiments(){
    using namespace EnvironmentControl;
    ListExperimentsRequest request;
    auto reply = requester_.SendRequest<ListExperimentsRequest, ListExperimentsReply>("ListExperiments", request, 1000);
    auto experiment_pb = reply.get();
    const auto& experiment_names = experiment_pb->experiment_names();
    return {experiment_names.begin(), experiment_names.end()};
}

std::string EnvironmentManager::EnvironmentController::GetQpidBrokerEndpoint(){
    using namespace EnvironmentControl;
    GetQpidBrokerEndpointRequest request;
    auto reply = requester_.SendRequest<GetQpidBrokerEndpointRequest, GetQpidBrokerEndpointReply>("GetQpidBrokerEndpoint", request, 1000);
    auto reply_pb = reply.get();
    return reply_pb->endpoint();
}

std::string EnvironmentManager::EnvironmentController::GetTaoCosnamingBrokerEndpoint(){
    using namespace EnvironmentControl;
    GetTaoCosnamingEndpointRequest request;
    auto reply = requester_.SendRequest<GetTaoCosnamingEndpointRequest, GetTaoCosnamingEndpointReply>("GetTaoCosnamingEndpoint", request, 1000);
    auto reply_pb = reply.get();
    return reply_pb->endpoint();
}

std::string EnvironmentManager::EnvironmentController::InspectExperiment(const std::string &experiment_name) {
    using namespace EnvironmentControl;

    NodeManager::InspectExperimentRequest request;
    request.set_experiment_name(experiment_name);
    auto reply = requester_.SendRequest<NodeManager::InspectExperimentRequest, NodeManager::InspectExperimentReply>("InspectExperiment", request, 1000);

    try{
        auto experiment_pb = reply.get();
        std::string output;

        google::protobuf::util::JsonPrintOptions options;
        options.add_whitespace = true;
        options.always_print_primitive_fields = false;
        google::protobuf::util::MessageToJsonString(*experiment_pb, &output, options);

        return output;
    }catch(const zmq::RMIException& ex){
        throw std::invalid_argument(ex.what());
    }catch(const zmq::TimeoutException& ex){
        throw std::runtime_error(ex.what());
    }

}


