#include "deploymenthandler.h"
#include <memory>
#include <utility>

namespace re::EnvironmentManager {
DeploymentHandler::DeploymentHandler(EnvironmentManager::Environment& env,
                                     const types::Ipv4& environment_manager_ip_address,
                                     EnvironmentManager::Environment::DeploymentType deployment_type,
                                     const types::Ipv4& deployment_ip_address,
                                     std::promise<uint16_t> port_promise,
                                     types::Uuid experiment_uuid) :
    environment_(env),
    environment_manager_ip_address_(environment_manager_ip_address),
    deployment_type_(deployment_type),
    deployment_ip_address_(deployment_ip_address),
    experiment_uuid_(std::move(experiment_uuid))
{
    std::lock_guard<std::mutex> lock(replier_mutex_);
    replier_ = std::make_unique<zmq::ProtoReplier>();
    const auto endpoint = environment_.GetDeploymentHandlerEndpoint(experiment_uuid_,
                                                                      deployment_type_);
    const auto bind_address = endpoint.tcp();

    try {
        replier_->Bind(bind_address);
        port_promise.set_value(endpoint.port());
    } catch(const zmq::error_t& ex) {
        std::runtime_error error("Cannot bind ZMQ address: '" + bind_address + "'");
        // Set our promise as exception and exit if we can't find a free port.
        port_promise.set_exception(std::make_exception_ptr(error));
        throw error;
    }

    // Register the callbacks
    replier_
        ->RegisterProtoCallback<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>(
            "NodeManagerHeartbeat",
            std::bind(&DeploymentHandler::HandleHeartbeat, this, std::placeholders::_1));
    replier_->RegisterProtoCallback<NodeManager::NodeManagerDeregistrationRequest,
                                    NodeManager::NodeManagerDeregistrationReply>(
        "NodeManagerDeregisteration",
        std::bind(&DeploymentHandler::HandleNodeManagerDeregisteration, this,
                  std::placeholders::_1));
    replier_
        ->RegisterProtoCallback<NodeManager::EnvironmentMessage, NodeManager::EnvironmentMessage>(
            "GetExperimentInfo",
            std::bind(&DeploymentHandler::HandleGetExperimentInfo, this, std::placeholders::_1));

    heartbeat_future_ = std::async(std::launch::async, &DeploymentHandler::HeartbeatLoop, this);
}

DeploymentHandler::~DeploymentHandler()
{
    {
        std::lock_guard<std::mutex> lock(replier_mutex_);
        if(replier_) {
            replier_.reset();
        }
    }
    if(heartbeat_future_.valid()) {
        heartbeat_future_.get();
    }
}

void DeploymentHandler::HeartbeatLoop() noexcept
{
    std::future<void> replier_future;
    {
        std::vector<std::chrono::milliseconds> timeouts;
        timeouts.emplace_back(2000);
        timeouts.emplace_back(4000);
        timeouts.emplace_back(8000);

        std::lock_guard<std::mutex> lock(replier_mutex_);
        replier_future = replier_->Start(timeouts);
    }

    try {
        replier_future.get();
    } catch(const zmq::TimeoutException& ex) {
        // Timeout Exceptions should lead to shutdown
        try {
            RemoveDeployment();
        } catch(const std::exception& ex) {
        }
    }
}

void DeploymentHandler::RemoveDeployment()
{
    std::lock_guard<std::mutex> lock(remove_mutex_);
    if(!removed_flag_) {
        if(deployment_type_ == EnvironmentManager::Environment::DeploymentType::EXECUTION_MASTER) {
            environment_.RemoveExperiment(experiment_uuid_);
        } else if(deployment_type_
                  == EnvironmentManager::Environment::DeploymentType::LOGAN_SERVER) {
            // Do nothing
        }
        removed_flag_ = true;
    }
}

std::unique_ptr<NodeManager::EnvironmentMessage>
DeploymentHandler::HandleHeartbeat(const NodeManager::EnvironmentMessage& request_message)
{
    if(request_message.type() == NodeManager::EnvironmentMessage::END_HEARTBEAT) {
        throw zmq::ShutdownException("Terminate Replier");
    }

    auto reply_message = std::make_unique<NodeManager::EnvironmentMessage>();
    reply_message->set_type(NodeManager::EnvironmentMessage::HEARTBEAT_ACK);

    if(deployment_type_ == EnvironmentManager::Environment::DeploymentType::EXECUTION_MASTER) {
        if(environment_.ExperimentIsDirty(experiment_uuid_)) {
            auto experiment_update = environment_.GetProto(experiment_uuid_, false);
            if(experiment_update) {
                reply_message.swap(experiment_update);
            }
        }
    } else if(deployment_type_ == EnvironmentManager::Environment::DeploymentType::LOGAN_SERVER) {
        // If we were registered, now we don't have the experiment anymore, Terminate
        if(!environment_.GotExperiment(experiment_uuid_)) {
            reply_message->set_type(NodeManager::EnvironmentMessage::SHUTDOWN_LOGAN_SERVER);
        }
    }
    return reply_message;
}

std::unique_ptr<NodeManager::NodeManagerDeregistrationReply>
DeploymentHandler::HandleNodeManagerDeregisteration(
    const NodeManager::NodeManagerDeregistrationRequest& request_message)
{
    RemoveDeployment();
    auto success = std::make_unique<NodeManager::NodeManagerDeregistrationReply>();
    return success;
}

std::unique_ptr<NodeManager::EnvironmentMessage>
DeploymentHandler::HandleGetExperimentInfo(const NodeManager::EnvironmentMessage& request_message)
{
    auto reply_message = std::make_unique<NodeManager::EnvironmentMessage>();

    auto& experiment = environment_.GetExperiment(experiment_uuid_);
    if(experiment.IsConfigured()) {
        auto experiment_update = environment_.GetProto(experiment_uuid_, true);
        if(experiment_update) {
            reply_message.swap(experiment_update);
        }
        experiment.SetActive();
    } else {
        throw std::runtime_error("Experiment: '" + experiment_uuid_.to_string() + "' already active");
    }
    return reply_message;
}

bool DeploymentHandler::IsRemovable() const
{
    std::lock_guard<std::mutex> lock(remove_mutex_);
    return removed_flag_;
}
} // namespace re::EnvironmentManager
