#include "experimentmanager.h"
#include <chrono>
#include <iostream>
#include <memory>
#include "helper.h"

namespace re::EnvironmentManager {
ExperimentManager::ExperimentManager(Experiment& experiment, const sem::types::Timeout& duration) :
    experiment_(experiment),
    experiment_manager_uuid_(sem::types::Uuid{}),
    master_publisher_endpoint_(experiment_.GetExperimentManagerPublisherEndpoint()),
    master_registration_endpoint_(experiment_.GetExperimentManagerRegistrationEndpoint()),
    experiment_logger_endpoint_(experiment_.GetExperimentLoggerEndpoint())
{
    // Setup our writer
    proto_writer_ = std::make_unique<zmq::ProtoWriter>();
    proto_writer_->BindPublisherSocket(master_publisher_endpoint_.tcp());

    experiment_logger_ = std::make_unique<Logan::ExperimentLogger>(
        experiment_.GetName(), experiment_logger_endpoint_.tcp(), ::Logger::Mode::LIVE);

    // Build our internal control message data structure.
    RequestDeployment();

    // Construct the execution future
    execution_future_ = std::async(std::launch::async, &ExperimentManager::ExecutionLoop, this,
                                   duration, execute_promise_.get_future(),
                                   terminate_promise_.get_future());

    // Bind Registration functions
    slave_registration_handler_ = std::make_unique<zmq::ProtoReplier>();
    slave_registration_handler_
        ->RegisterProtoCallback<NodeManager::SlaveStartupRequest, NodeManager::SlaveStartupReply>(
            "SlaveStartup",
            std::bind(&ExperimentManager::HandleSlaveStartup, this, std::placeholders::_1));

    slave_registration_handler_->RegisterProtoCallback<NodeManager::SlaveConfiguredRequest,
                                                       NodeManager::SlaveConfiguredReply>(
        "SlaveConfigured",
        std::bind(&ExperimentManager::HandleSlaveConfigured, this, std::placeholders::_1));

    slave_registration_handler_->RegisterProtoCallback<NodeManager::SlaveTerminatedRequest,
                                                       NodeManager::SlaveTerminatedReply>(
        "SlaveTerminated",
        std::bind(&ExperimentManager::HandleSlaveTerminated, this, std::placeholders::_1));
    slave_registration_handler_->RegisterProtoCallback<NodeManager::SlaveHeartbeatRequest,
                                                       NodeManager::SlaveHeartbeatReply>(
        "SlaveHeartbeat",
        std::bind(&ExperimentManager::HandleSlaveHeartbeat, this, std::placeholders::_1));
    slave_registration_handler_->Bind("tcp://" + master_registration_endpoint_.to_string());
    slave_registration_handler_->Start();
};

ExperimentManager::~ExperimentManager()
{
    Terminate();
}

void ExperimentManager::Terminate()
{
    auto error = std::runtime_error("Shutdown");
    try {
        execute_promise_.set_exception(std::make_exception_ptr(error));
    } catch(const std::exception& ex) {
    }

    try {
        terminate_promise_.set_exception(std::make_exception_ptr(error));
    } catch(const std::exception& ex) {
    }

    if(execution_future_.valid()) {
        try {
            execution_future_.get();
        } catch(const std::exception& ex) {
            std::cerr << "ExperimentManager::Terminate: " << ex.what() << std::endl;
        }
    }
}

// Get experiment deployment info from environment. Save to control message data structure.
void ExperimentManager::RequestDeployment()
{
    using namespace NodeManager;
    auto environment_message = std::make_unique<NodeManager::EnvironmentMessage>();

    if(experiment_.IsConfigured()) {
        auto experiment_update = experiment_.GetProto(true);
        if(experiment_update) {
            environment_message.swap(experiment_update);
        }
        experiment_.SetActive();
    } else {
        throw std::runtime_error("Experiment: '" + experiment_.GetName() + "' already active");
    }

    // Take a copy of the control message
    std::lock_guard<std::mutex> lock(control_message_mutex_);
    control_message_ = std::make_unique<ControlMessage>(environment_message->control_message());

    std::lock_guard<std::mutex> slave_lock(slave_state_mutex_);
    // Construct a set of slaves to wait
    auto now = std::chrono::steady_clock::now();

    for(const auto& node : control_message_->nodes()) {
        for(const auto& container : node.containers()) {
            const auto& slave_key = GetSlaveKey(node.ip_address(), container.info().id());

            auto insert = slave_status_.emplace(slave_key, SlaveStatus{SlaveState::OFFLINE, now});
            if(!insert.second) {
                throw std::runtime_error("Got duplicate slaves in ControlMessage with IP: '"
                                         + slave_key + "'");
            }
        }
    }
}

void ExperimentManager::UpdateExperiment() {
    using namespace NodeManager;
    auto environment_message = std::make_unique<NodeManager::EnvironmentMessage>();

    if(experiment_.IsActive()) {
        auto experiment_update = experiment_.GetProto(true);
        if(experiment_update) {
            environment_message.swap(experiment_update);
        }
    } else {
        throw std::runtime_error("Experiment: '" + experiment_.GetName() + "' is not active.");
    }

    // Take a copy of the control message
    std::lock_guard<std::mutex> lock(control_message_mutex_);
    control_message_ = std::make_unique<ControlMessage>(environment_message->control_message());

    std::lock_guard<std::mutex> slave_lock(slave_state_mutex_);
    // Construct a set of slaves to wait
    auto now = std::chrono::steady_clock::now();

    for(const auto& node : control_message_->nodes()) {
        for(const auto& container : node.containers()) {
            const auto& slave_key = GetSlaveKey(node.ip_address(), container.info().id());
            slave_status_.emplace(slave_key, SlaveStatus{SlaveState::OFFLINE, now});
        }
    }
}

ExperimentManager::SlaveState ExperimentManager::GetSlaveState(const std::string& slave_key)
{
    return GetSlaveStatus(slave_key).state;
}

void ExperimentManager::SetSlaveState(const std::string& slave_key, SlaveState state)
{
    GetSlaveStatus(slave_key).state = state;
    HandleSlaveStateChange();
}

void ExperimentManager::SetSlaveAlive(const std::string& slave_key)
{
    GetSlaveStatus(slave_key).heartbeat_time = std::chrono::steady_clock::now();
}

ExperimentManager::SlaveStatus& ExperimentManager::GetSlaveStatus(const std::string& slave_key)
{
    if(slave_status_.count(slave_key)) {
        return slave_status_.at(slave_key);
    } else if(late_joiner_slave_status_.count(slave_key)) {
        return late_joiner_slave_status_.at(slave_key);
    } else {
        throw std::runtime_error("Slave with Key: '" + slave_key + "' not in experiment");
    }
}

int ExperimentManager::GetSlaveStateCount(const SlaveState& state)
{
    int count = 0;
    for(const auto& ss : slave_status_) {
        if(ss.second.state == state) {
            count++;
        }
    }
    return count;
}

void ExperimentManager::HandleSlaveStateChange()
{
    // Whats our executions state
    auto slave_count = slave_status_.size();

    auto configured_count = GetSlaveStateCount(SlaveState::CONFIGURED);
    auto terminated_count = GetSlaveStateCount(SlaveState::TERMINATED);
    auto error_count = GetSlaveStateCount(SlaveState::ERROR_);

    if(configured_count == slave_count && !started_) {
        // All Slaves are configured, so Start
        // REVIEW (Mitch): This future/promise pair is getting used as a semaphore, this is a bad D:
        execute_promise_.set_value();
        started_ = true;
    } else if((configured_count + error_count) == slave_count && !started_) {
        // All Slaves are either configured or have errored, so fail
        std::runtime_error error(std::to_string(error_count) + " slaves has errors.");
        execute_promise_.set_exception(std::make_exception_ptr(error));
    }
    slave_state_cv_.notify_all();
}

// REVIEW (Mitch): This function should be called AllSlavesTerminatedOrTimedOut
// REVIEW (Mitch): This function also hides any particular slave failure on heartbeat timeout
bool ExperimentManager::AllSlavesTerminated()
{
    const auto now = std::chrono::steady_clock::now();

    if(execution_valid_) {
        // REVIEW (Mitch): Re-designing this algorithm's control flow is probably a good idea
        for(const auto& ss : slave_status_) {
            if(ss.second.state == SlaveState::TERMINATED) {
                continue;
            }
            return false;
        }
    }
    return true;
}

std::string ExperimentManager::GetSlaveKey(const std::string& ip, const std::string& container_id)
{
    return ip + "_" + container_id;
}

std::string ExperimentManager::GetSlaveKey(const NodeManager::SlaveId& slave)
{
    return GetSlaveKey(slave.ip_address(), slave.container_id());
}

// REVIEW (Mitch): Slave operation failure should be handled when they're encountered. Currently
//  we're waiting till we have messages from all slaves before acting on any one failure.
std::unique_ptr<NodeManager::SlaveHeartbeatReply>
ExperimentManager::HandleSlaveHeartbeat(const NodeManager::SlaveHeartbeatRequest& request)
{
    using namespace NodeManager;
    const auto& slave_key = GetSlaveKey(request.id());
    {
        std::lock_guard<std::mutex> slave_lock(slave_state_mutex_);
        SetSlaveAlive(slave_key);
    }
    return std::make_unique<SlaveHeartbeatReply>();
}

std::unique_ptr<NodeManager::SlaveStartupReply>
ExperimentManager::HandleSlaveStartup(const NodeManager::SlaveStartupRequest& request)
{
    using namespace NodeManager;
    std::unique_ptr<SlaveStartupReply> reply;
    const auto& slave_key = GetSlaveKey(request.id());

    std::lock_guard<std::mutex> slave_lock(slave_state_mutex_);
    if(GetSlaveState(slave_key) == SlaveState::OFFLINE) {
        std::lock_guard<std::mutex> control_lock(control_message_mutex_);

        for(const auto& node : control_message_->nodes()) {
            if(request.id().ip_address() == node.ip_address()) {
                for(const auto& container : node.containers()) {
                    if(request.id().container_id() == container.info().id()) {
                        // Give them there right container
                        reply = std::make_unique<SlaveStartupReply>();
                        reply->set_host_name(node.info().name());
                        reply->set_allocated_configuration(new NodeManager::Container(container));
                        // Set the slave state as Registered
                        SetSlaveState(slave_key, SlaveState::REGISTERED);
                    }
                }
            }
        }
    } else {
        throw std::runtime_error("Slave with Key: '" + slave_key + "' not in correct state");
    }
    return reply;
}

std::unique_ptr<NodeManager::SlaveConfiguredReply>
ExperimentManager::HandleSlaveConfigured(const NodeManager::SlaveConfiguredRequest& request)
{
    using namespace NodeManager;
    std::unique_ptr<SlaveConfiguredReply> reply;
    const auto& slave_key = GetSlaveKey(request.id());

    std::lock_guard<std::mutex> slave_lock(slave_state_mutex_);
    if(GetSlaveState(slave_key) == SlaveState::REGISTERED) {
        reply = std::make_unique<SlaveConfiguredReply>();
        auto slave_state = SlaveState::CONFIGURED;
        if(request.success()) {
            Log({"DeploymentContainer Online: " + slave_key});
        } else {
            std::string errors{};
            for(const auto& error_str : request.error_messages()) {
                errors += error_str;
            }
            Log({{"DeploymentContainer: " + slave_key},
                 {"Failed to start with errors: {" + errors + "}"}});
            slave_state = SlaveState::ERROR_;
        }
        // Set the slave state as Configured or Error'd
        SetSlaveState(slave_key, slave_state);
    } else {
        throw std::runtime_error("DeploymentContainer on IP: '" + slave_key
                                 + "' not in correct state");
    }
    return reply;
}

std::unique_ptr<NodeManager::SlaveTerminatedReply>
ExperimentManager::HandleSlaveTerminated(const NodeManager::SlaveTerminatedRequest& request)
{
    using namespace NodeManager;
    const auto& slave_key = GetSlaveKey(request.id());

    std::lock_guard<std::mutex> slave_lock(slave_state_mutex_);
    auto reply = std::make_unique<SlaveTerminatedReply>();
    SetSlaveState(slave_key, SlaveState::TERMINATED);
    return reply;
}

void ExperimentManager::PushControlMessage(const std::string& topic,
                                           std::unique_ptr<NodeManager::ControlMessage> message)
{
    if(proto_writer_) {
        proto_writer_->PushMessage(topic, std::move(message));
    }
}

void ExperimentManager::HandleExperimentUpdate(
    const NodeManager::EnvironmentMessage& environment_update)
{
    using namespace NodeManager;
    // TODO: filter only nodes we want to update???
    // can we even do that??
    const auto& type = environment_update.type();

    switch(type) {
        case EnvironmentMessage::CONFIGURE_EXPERIMENT: {
            auto control_message = std::make_unique<ControlMessage>(
                environment_update.control_message());
            PushControlMessage("*", std::move(control_message));
            break;
        }
        case EnvironmentMessage::SHUTDOWN_EXPERIMENT: {
            break;
        }
        default: {
            throw std::runtime_error("Unhandled EnvironmentMessage Type: "
                                     + EnvironmentMessage_Type_Name(type));
        }
    }
}

std::unique_ptr<NodeManager::ControlMessage>
ExperimentManager::ConstructStateControlMessage(NodeManager::ControlMessage::Type type)
{
    auto message = std::make_unique<NodeManager::ControlMessage>();
    message->set_type(type);
    return message;
}

::Logger::LifeCycleEvent TranslateLifecycle(const NodeManager::ControlMessage::Type& state)
{
    switch(state) {
        case NodeManager::ControlMessage::ACTIVATE:
            return ::Logger::LifeCycleEvent::ACTIVATED;
        case NodeManager::ControlMessage::CONFIGURE:
            return ::Logger::LifeCycleEvent::CONFIGURED;
        case NodeManager::ControlMessage::PASSIVATE:
            return ::Logger::LifeCycleEvent::PASSIVATED;
        case NodeManager::ControlMessage::TERMINATE:
            return ::Logger::LifeCycleEvent::TERMINATED;
        default:
            throw std::runtime_error("Unhandled State");
    }
}

void ExperimentManager::PushStateChange(const NodeManager::ControlMessage::Type& state)
{
    auto state_change_name = NodeManager::ControlMessage::Type_Name(state);
    Log({"Experiment state change: " + state_change_name});
    PushControlMessage("*", ConstructStateControlMessage(state));
    try {
        auto lifecycle_event = TranslateLifecycle(state);
        if(experiment_logger_) {
            experiment_logger_->LogLifecycleEvent(lifecycle_event);
        }
    } catch(const std::exception& ex) {
        // REVIEW (Mitch): Do something with this exception
        Log({{"Experiment state change: " + state_change_name},
             {"Failed with error: " + std::string(ex.what())}});
    }
}

void ExperimentManager::ExecutionLoop(const sem::types::Timeout& duration,
                                      std::future<void> execute_future,
                                      std::future<void> terminate_future)
{
    Log({"Starts"});
    using namespace NodeManager;

    bool should_execute = true;
    try {
        if(execute_future.valid()) {
            // REVIEW (Mitch): this future is used as a semaphore. It gets by a call to
            //  execute_promise_.set_value(). Use a condition variable here?
            execute_future.get();
            // Sleep to allow time for the publisher to be bound
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::lock_guard<std::mutex> lock(slave_state_mutex_);
        execution_valid_ = true;
    } catch(const std::exception& ex) {
        should_execute = false;
    }

    if(should_execute) {
        for(int i = 3; i >= 1; i--) {
            Log({"Activating experiment in " + std::to_string(i) + " second(s)"});
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        PushStateChange(ControlMessage::ACTIVATE);

        if(terminate_future.valid()) {
            std::future_status status = std::future_status::ready;
            if(std::holds_alternative<std::chrono::milliseconds>(duration)) {
                // Sleep for the predetermined time
                auto run_time_ms = std::get<std::chrono::milliseconds>(duration);
                Log({"Running for " + std::to_string(run_time_ms.count()) + "ms"});
                status = terminate_future.wait_for(run_time_ms);
            }

            if(status == std::future_status::ready) {
                try {
                    // REVIEW (Mitch): what can this throw and under which circumstances?
                    terminate_future.get();
                } catch(const std::exception& ex) {
                }
            }
        }
        PushStateChange(ControlMessage::PASSIVATE);
    }

    PushStateChange(ControlMessage::TERMINATE);

    Log({"De-registering Deployment Containers"});
    {
        while(true) {
            std::unique_lock<std::mutex> slave_lock(slave_state_mutex_);
            // Wait for all slaves to be offline or timed out
            // REVIEW (Mitch): Why the random 1 second timeout here?
            if(slave_state_cv_.wait_for(slave_lock, std::chrono::seconds(10),
                                        [this] { return AllSlavesTerminated(); })) {
                break;
            }
        }
    }

    Log({"Ends"});
    experiment_.SetFinished();
}
auto ExperimentManager::Log(const std::vector<std::string>& messages) -> void
{
    std::cout << "[ExperimentManager]  - {" << experiment_.GetUuid() << "} " << messages.front();
    if(messages.size() == 1) {
        std::cout << std::endl;
        return;
    }
    for(auto iter = std::begin(messages) + 1, end = std::end(messages); iter != end; iter++) {
        std::cout << "\n                     - " << *iter;
    }
    std::cout << std::endl;
}
} // namespace re::EnvironmentManager
