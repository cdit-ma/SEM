#include "environmentcontroller.h"
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
#include <google/protobuf/util/json_util.h>
#include "environmentcontrol.pb.h"
#ifdef _WIN32
#pragma warning(pop)
#endif
#include "protobufmodelparser.h"
#include "experimentdefinition.h"
#include <fstream>
#include "modelparser.h"

EnvironmentManager::EnvironmentController::EnvironmentController(
    const std::string& environment_manager_endpoint) :
    requester_(environment_manager_endpoint)
{
}

std::vector<std::string>
EnvironmentManager::EnvironmentController::ShutdownExperiment(const std::string& experiment_name,
                                                              bool is_regex)
{
    using namespace EnvironmentControl;
    ShutdownExperimentRequest request;
    request.set_experiment_name(experiment_name);
    request.set_is_regex(is_regex);
    auto reply = requester_.SendRequest<ShutdownExperimentRequest, ShutdownExperimentReply>(
        "ShutdownExperiment", request, standard_timeout.count());
    auto experiment_pb = reply.get();
    const auto& experiment_names = experiment_pb->experiment_names();
    return {experiment_names.begin(), experiment_names.end()};
}

std::unique_ptr<re::network::protocol::experimentdefinition::RegistrationReply>
EnvironmentManager::EnvironmentController::AddExperiment(const std::string& experiment_name,
                                                         const std::string& graphml_path,
                                                         int experiment_duration)
{
    using Request = re::network::protocol::experimentdefinition::RegistrationRequest;
    using Reply = re::network::protocol::experimentdefinition::RegistrationReply;

    Request request;
    request.set_request_uuid(re::types::Uuid{}.to_string());

    try {
        std::ifstream model_stream(graphml_path);
        re::Representation::ExperimentDefinition definition(model_stream);
        auto deployed_model = re::ModelParser::ModelParser::ParseModel(
            graphml_path, definition.GetUuid().to_string());
        definition.SetName(experiment_name);


        deployed_model->set_name(experiment_name);
        deployed_model->set_uuid(definition.GetUuid().to_string());
        deployed_model->set_duration_seconds(experiment_duration);

        request.set_allocated_experiment(definition.ToProto().release());
        request.set_allocated_old_experiment_message_type(deployed_model.release());
        request.set_duration_seconds(experiment_duration);
    } catch(const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        throw;
    }

    auto reply = requester_.SendRequest<Request, Reply>("RegisterExperiment", request,
                                                        standard_timeout.count());
    return reply.get();
}

std::vector<std::string> EnvironmentManager::EnvironmentController::ListExperiments()
{
    using namespace EnvironmentControl;
    ListExperimentsRequest request;
    auto reply = requester_.SendRequest<ListExperimentsRequest, ListExperimentsReply>(
        "ListExperiments", request, standard_timeout.count());
    auto experiment_pb = reply.get();
    const auto& experiment_names = experiment_pb->experiment_names();
    return {experiment_names.begin(), experiment_names.end()};
}

std::string EnvironmentManager::EnvironmentController::GetQpidBrokerEndpoint()
{
    using namespace EnvironmentControl;
    GetQpidBrokerEndpointRequest request;
    auto reply = requester_.SendRequest<GetQpidBrokerEndpointRequest, GetQpidBrokerEndpointReply>(
        "GetQpidBrokerEndpoint", request, standard_timeout.count());
    auto reply_pb = reply.get();
    return reply_pb->endpoint();
}

std::string EnvironmentManager::EnvironmentController::GetTaoCosnamingBrokerEndpoint()
{
    using namespace EnvironmentControl;
    GetTaoCosnamingEndpointRequest request;
    auto reply =
        requester_.SendRequest<GetTaoCosnamingEndpointRequest, GetTaoCosnamingEndpointReply>(
            "GetTaoCosnamingEndpoint", request, standard_timeout.count());
    auto reply_pb = reply.get();
    return reply_pb->endpoint();
}

std::string
EnvironmentManager::EnvironmentController::InspectExperiment(const std::string& experiment_name)
{
    using namespace EnvironmentControl;

    NodeManager::InspectExperimentRequest request;
    request.set_experiment_name(experiment_name);
    auto reply = requester_.SendRequest<NodeManager::InspectExperimentRequest,
                                        NodeManager::InspectExperimentReply>(
        "InspectExperiment", request, standard_timeout.count());

    try {
        auto experiment_pb = reply.get();
        std::string output;

        google::protobuf::util::JsonPrintOptions options;
        options.add_whitespace = true;
        options.always_print_primitive_fields = false;
        google::protobuf::util::MessageToJsonString(*experiment_pb, &output, options);

        return output;
    } catch(const zmq::RMIException& ex) {
        throw std::invalid_argument(ex.what());
    } catch(const zmq::TimeoutException& ex) {
        throw std::runtime_error(ex.what());
    }
}
auto EnvironmentManager::EnvironmentController::StartExperiment(re::types::Uuid experiment_uuid)
    -> bool
{
    using namespace EnvironmentControl;
    StartExperimentRequest request;
    request.set_experiment_uuid(experiment_uuid.to_string());

    auto reply = requester_.SendRequest<StartExperimentRequest, StartExperimentReply>(
        "StartExperiment", request, 5000);

    return reply.get()->success();
}
