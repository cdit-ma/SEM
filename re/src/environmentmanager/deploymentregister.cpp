#include "deploymentregister.h"
#include "environment/container.h"
#include "environment/logger.h"
#include "environment/node.h"
#include "experimentmanager.h"
#include <chrono>
#include <exception>
#include "experimentdefinition.h"
#include <memory>
#include "helper.h"
#include <zmq.hpp>

namespace re::EnvironmentManager {
DeploymentRegister::DeploymentRegister(const types::Ipv4& environment_manager_ip_address,
                                       uint16_t registration_port,
                                       const types::SocketAddress& qpid_broker_address,
                                       const std::string& tao_naming_server_address,
                                       uint16_t port_range_min,
                                       uint16_t port_range_max) :
    environment_manager_ip_address_(environment_manager_ip_address),
    node_manager_registry_{qpid_broker_address}
{
    assert(port_range_min < port_range_max);
    assert(std::numeric_limits<uint16_t>::max() - port_range_max > 10000);

    types::unique_queue<uint16_t> default_node_port_pool_;
    for(uint16_t i = port_range_min; i < port_range_max; i++) {
        default_node_port_pool_.push(i);
    }

    types::unique_queue<uint16_t> environment_manager_port_pool_;
    for(uint16_t i = port_range_min + 10000; i < port_range_max + 10000; i++) {
        environment_manager_port_pool_.push(i);
    }

    environment_ = std::make_unique<EnvironmentManager::Environment>(
        environment_manager_ip_address, qpid_broker_address, tao_naming_server_address,
        environment_manager_port_pool_, default_node_port_pool_);

    replier_ = std::make_unique<zmq::ProtoReplier>();
    auto registration_endpoint = types::SocketAddress(environment_manager_ip_address,
                                                                 registration_port);
    replier_->Bind(registration_endpoint.tcp());

    // Logan Managed Server Functions
    replier_->RegisterProtoCallback<NodeManager::LoganRegistrationRequest,
                                    NodeManager::LoganRegistrationReply>(
        "LoganRegistration",
        std::bind(&DeploymentRegister::HandleLoganRegistration, this, std::placeholders::_1));

    // Controller Functions
    replier_->RegisterProtoCallback<re::network::protocol::experimentdefinition::RegistrationRequest,
                                    re::network::protocol::experimentdefinition::RegistrationReply>(
        "RegisterExperiment",
        std::bind(&DeploymentRegister::HandleRegisterExperiment, this, std::placeholders::_1));

    replier_->RegisterProtoCallback<EnvironmentControl::StartExperimentRequest,
                                    EnvironmentControl::StartExperimentReply>(
        "StartExperiment", [this](const EnvironmentControl::StartExperimentRequest& request) {
            return DeploymentRegister::HandleStartExperiment(request);
        });

    replier_->RegisterProtoCallback<EnvironmentControl::ShutdownExperimentRequest,
                                    EnvironmentControl::ShutdownExperimentReply>(
        "ShutdownExperiment",
        std::bind(&DeploymentRegister::HandleShutdownExperiment, this, std::placeholders::_1));

    replier_->RegisterProtoCallback<EnvironmentControl::ListExperimentsRequest,
                                    EnvironmentControl::ListExperimentsReply>(
        "ListExperiments",
        std::bind(&DeploymentRegister::HandleListExperiments, this, std::placeholders::_1));

    replier_->RegisterProtoCallback<NodeManager::InspectExperimentRequest,
                                    NodeManager::InspectExperimentReply>(
        "InspectExperiment",
        std::bind(&DeploymentRegister::HandleInspectExperiment, this, std::placeholders::_1));

    // Aggregation server functions
    replier_->RegisterProtoCallback<NodeManager::AggregationServerRegistrationRequest,
                                    NodeManager::AggregationServerRegistrationReply>(
        "AggregationServerRegistration",
        [this](const NodeManager::AggregationServerRegistrationRequest& message) {
            return DeploymentRegister::HandleAggregationServerRegistration(message);
        });

    // Medea request functions
    replier_->RegisterProtoCallback<NodeManager::MEDEAInterfaceRequest,
                                    NodeManager::MEDEAInterfaceReply>(
        "MEDEAInterfaceRequest", [this](const NodeManager::MEDEAInterfaceRequest& message) {
            return DeploymentRegister::HandleMEDEAInterfaceRequest(message);
        });

    replier_->RegisterProtoCallback<EnvironmentControl::GetQpidBrokerEndpointRequest,
                                    EnvironmentControl::GetQpidBrokerEndpointReply>(
        "GetQpidBrokerEndpoint",
        [this](const EnvironmentControl::GetQpidBrokerEndpointRequest& message) {
            return DeploymentRegister::GetQpidBrokerEndpoint(message);
        });

    replier_->RegisterProtoCallback<EnvironmentControl::GetTaoCosnamingEndpointRequest,
                                    EnvironmentControl::GetTaoCosnamingEndpointReply>(
        "GetTaoCosnamingEndpoint",
        [this](const EnvironmentControl::GetTaoCosnamingEndpointRequest& message) {
            return DeploymentRegister::GetTaoCosnamingEndpoint(message);
        });

    replier_->Start();

    cleanup_future_ = std::async(std::launch::async, &DeploymentRegister::CleanupTask, this);
    Log({"Started"});
}

DeploymentRegister::~DeploymentRegister()
{
    Terminate();
    environment_.reset();
    Log({"Shutdown"});
}

void DeploymentRegister::Terminate()
{
    if(replier_) {
        replier_->Terminate();
        replier_.reset();
    }

    {
        std::lock_guard<std::mutex> lock(termination_mutex_);
        terminated_ = true;
        cleanup_cv_.notify_all();
    }

    if(cleanup_future_.valid()) {
        cleanup_future_.get();
    }

    {
        std::lock_guard<std::mutex> lock(handler_mutex_);
        handlers_.clear();
    }
}

std::unique_ptr<NodeManager::LoganRegistrationReply>
DeploymentRegister::HandleLoganRegistration(const NodeManager::LoganRegistrationRequest& request)
{
    const auto& experiment_uuid = types::Uuid{request.id().experiment_uuid()};
    const auto logan_server_ip_address = types::Ipv4(request.id().ip_address());

    std::promise<uint16_t> port_promise;
    auto port_future = port_promise.get_future();

    if(!environment_->GotExperiment(experiment_uuid)) {
        throw std::runtime_error("No experiment with name: '" + request.id().experiment_name() + "("
                                 + experiment_uuid.to_string() + ")'");
    }

    try {
        auto& experiment = environment_->GetExperiment(experiment_uuid);
        auto logging_servers = experiment.GetAllocatedLoganServers(
            logan_server_ip_address.to_string());

        auto reply = std::make_unique<NodeManager::LoganRegistrationReply>();

        if(!logging_servers.empty()) {
            auto handler = std::make_unique<DeploymentHandler>(
                *environment_, environment_manager_ip_address_,
                EnvironmentManager::Environment::DeploymentType::LOGAN_SERVER,
                logan_server_ip_address, std::move(port_promise), experiment_uuid);
            {
                std::lock_guard<std::mutex> lock(handler_mutex_);
                handlers_.emplace_back(std::move(handler));
            }
            // Wait for port assignment from heartbeat loop, .get() will throw if out of ports.
            auto heartbeat_endpoint =
                types::SocketAddress(environment_manager_ip_address_, port_future.get());
            reply->set_heartbeat_endpoint(heartbeat_endpoint.tcp());
        }

        for(auto& logger : logging_servers) {
            reply->mutable_logger()->AddAllocated(logger.release());
        }

        return std::move(reply);
    } catch(const std::exception& ex) {
        throw;
    }
}

std::unique_ptr<EnvironmentControl::ShutdownExperimentReply>
DeploymentRegister::HandleShutdownExperiment(
    const EnvironmentControl::ShutdownExperimentRequest& message)
{
    auto reply_message = std::make_unique<EnvironmentControl::ShutdownExperimentReply>();

    if(message.is_regex()) {
        auto experiment_uuids = environment_->ShutdownExperimentRegex(message.experiment_name());
        for(const auto uuid : experiment_uuids) {
            reply_message->mutable_experiment_names()->Add(uuid.to_string());
        }
    } else {
        auto uuid = environment_->GetExperimentUuid(message.experiment_name());
        environment_->ShutdownExperiment(uuid);
        reply_message->add_experiment_names(message.experiment_name());
    }
    return reply_message;
}

std::unique_ptr<EnvironmentControl::ListExperimentsReply>
DeploymentRegister::HandleListExperiments(const EnvironmentControl::ListExperimentsRequest& message)
{
    auto reply_message = std::make_unique<EnvironmentControl::ListExperimentsReply>();
    auto experiment_names = environment_->GetExperimentNames();
    *reply_message->mutable_experiment_names() = {experiment_names.begin(), experiment_names.end()};
    return reply_message;
}

/** This is our "Add new experiment" entry point. This callback is triggered when we receive a
 * RegisterExperimentRequest from the EnvironmentController.
 * We should:
 * * Populate our internal representation of the experiment.
 * * Start up an ExperimentManager
 * * Request that required NodeManagers start EPMs for this experiment.
 * * Reply with our deployment information (? legacy)
 */
std::unique_ptr<re::network::protocol::experimentdefinition::RegistrationReply>
DeploymentRegister::HandleRegisterExperiment(
    const re::network::protocol::experimentdefinition::RegistrationRequest& request)
{
    Log({"Received registration request.",
         "Attempting to register experiment: " + request.experiment().experiment_name() + " {"
             + request.experiment().uuid() + "}"});
    auto&& experiment_name = request.experiment().experiment_name();
    re::Representation::ExperimentDefinition definition(request.experiment());
    auto experiment_uuid = types::Uuid{definition.GetUuid()};

    auto out = std::make_unique<re::network::protocol::experimentdefinition::RegistrationReply>();
    out->set_experiment_uuid(definition.GetUuid().to_string());

    // Build our internal representation of the experiment to run.
    environment_->PopulateExperiment(node_manager_registry_, request.old_experiment_message_type(),
                                     std::move(definition));
    Log({"Added new experiment: " + experiment_name + " {" + experiment_uuid.to_string() + "}",
         "Current registered experiments: " + std::to_string(environment_->GetExperimentCount())});

    auto& experiment = environment_->GetExperiment(experiment_uuid);

    // Respond with our deployment information
    auto experiment_deployment_info = environment_->GetExperimentDeploymentInfo(experiment_uuid);
    return out;
}

auto DeploymentRegister::HandleStartExperiment(
    const EnvironmentControl::StartExperimentRequest& request)
    -> std::unique_ptr<EnvironmentControl::StartExperimentReply>
{
    const auto experiment_uuid = types::Uuid{request.experiment_uuid()};

    auto& experiment = environment_->GetExperiment(experiment_uuid);
    try {
        // Start EPMs on nodes required
        experiment.StartExperimentProcesses();
        auto reply = std::make_unique<EnvironmentControl::StartExperimentReply>();
        reply->set_success(true);
        return reply;
    } catch(const std::exception& ex) {
        environment_->RemoveExperiment(experiment_uuid);
        auto reply = std::make_unique<EnvironmentControl::StartExperimentReply>();
        reply->set_success(false);
        return reply;
    }
}

std::unique_ptr<NodeManager::AggregationServerRegistrationReply>
DeploymentRegister::HandleAggregationServerRegistration(
    const NodeManager::AggregationServerRegistrationRequest& request)
{
    auto publisher_endpoint = environment_->GetUpdatePublisherEndpoint();
    auto reply = std::make_unique<NodeManager::AggregationServerRegistrationReply>();
    reply->set_publisher_endpoint(publisher_endpoint.tcp());

    return reply;
}

std::unique_ptr<NodeManager::MEDEAInterfaceReply>
DeploymentRegister::HandleMEDEAInterfaceRequest(const NodeManager::MEDEAInterfaceRequest& message)
{
    auto reply_message = std::make_unique<NodeManager::MEDEAInterfaceReply>();

    auto experiment_names = environment_->GetExperimentNames();
    *reply_message->mutable_experiment_names() = {experiment_names.begin(), experiment_names.end()};

    auto external_ports = environment_->GetExternalPorts();

    for(auto& external_port : external_ports) {
        reply_message->mutable_external_ports()->AddAllocated(external_port.release());
    }

    return reply_message;
}

// TODO: Fix this, invalidates the dirty flag on experiments.
std::unique_ptr<NodeManager::InspectExperimentReply>
DeploymentRegister::HandleInspectExperiment(const NodeManager::InspectExperimentRequest& request)
{
    auto reply = std::make_unique<NodeManager::InspectExperimentReply>();
    reply->set_allocated_environment_message(
        environment_->GetProto(environment_->GetExperimentUuid(request.experiment_name()), true)
            .release());
    return reply;
}

void DeploymentRegister::CleanupTask()
{
    while(true) {
        std::unique_lock<std::mutex> termination_lock(termination_mutex_);
        cleanup_cv_.wait_for(termination_lock, cleanup_period_, [this] { return terminated_; });

        if(terminated_) {
            break;
        }

        {
            std::lock_guard<std::mutex> handler_lock(handler_mutex_);
            handlers_.erase(std::remove_if(handlers_.begin(), handlers_.end(),
                                           [](const std::unique_ptr<DeploymentHandler>& handler) {
                                               return handler->IsRemovable();
                                           }),
                            handlers_.end());
            environment_->CleanUpExperiments();
        }
    }
}

std::unique_ptr<EnvironmentControl::GetQpidBrokerEndpointReply>
DeploymentRegister::GetQpidBrokerEndpoint(
    const EnvironmentControl::GetQpidBrokerEndpointRequest& message)
{
    auto reply = std::make_unique<EnvironmentControl::GetQpidBrokerEndpointReply>();

    reply->set_endpoint(environment_->GetAmqpBrokerAddress().to_string());
    return reply;
}

std::unique_ptr<EnvironmentControl::GetTaoCosnamingEndpointReply>
DeploymentRegister::GetTaoCosnamingEndpoint(
    const EnvironmentControl::GetTaoCosnamingEndpointRequest& message)
{
    auto reply = std::make_unique<EnvironmentControl::GetTaoCosnamingEndpointReply>();

    reply->set_endpoint(environment_->GetTaoNamingServiceAddress());
    return reply;
}

auto DeploymentRegister::Log(const std::vector<std::string>& messages) const -> void
{
    std::cout << FormatLogMessage(messages) << std::endl;
}

auto DeploymentRegister::LogError(const std::vector<std::string>& messages) const -> void
{
    std::cerr << FormatLogMessage(messages) << std::endl;
}

auto DeploymentRegister::FormatLogMessage(const std::vector<std::string>& messages) const
    -> std::string
{
    std::string output;
    output = "[DeploymentRegister] - " + messages.front();
    if(messages.size() == 1) {
        return output;
    }

    // Use annoying for loop to start from second element
    for(auto iter = std::begin(messages) + 1, end = std::end(messages); iter != end; iter++) {
        output += ("\n                     - " + *iter);
    }
    return output;
}
} // namespace re::EnvironmentManager
