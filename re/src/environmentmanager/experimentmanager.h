#ifndef EXECUTIONMANAGER_H
#define EXECUTIONMANAGER_H

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "environmentrequester.h"
#include "experiment_logger.h"
#include "environment/environment.h"
#include "controlmessage.pb.h"
#include "socketaddress.hpp"
#include "timeout.hpp"
#include "uuid.h"
#include "execution.hpp"
#include "protoreplier.hpp"
#include "protowriter.h"

// REVIEW (Mitch): Remove forward decl
namespace zmq {
class ProtoWriter;
};

// REVIEW (Mitch): DOCUMENTATION: Exactly one of these is started per experiment.
namespace re::EnvironmentManager {
class ExperimentManager {
public:
    // REVIEW (Mitch): Use endpoint class
    ExperimentManager(Experiment& experiment,
                     const sem::types::Timeout& duration);
    ~ExperimentManager();
    void UpdateExperiment();

    void PushStateChange(const NodeManager::ControlMessage::Type& state);
private:
    enum class SlaveState {
        OFFLINE = 0,
        REGISTERED = 1,
        CONFIGURED = 2,
        TERMINATED = 3,
        ERROR_ = 4
    };

    struct SlaveStatus {
        SlaveState state;
        std::chrono::steady_clock::time_point heartbeat_time;
    };

    static std::string GetSlaveKey(const std::string& ip, const std::string& container_id);
    static std::string GetSlaveKey(const NodeManager::SlaveId& slave);
    bool AllSlavesTerminated();
    void RequestDeployment();
    void Terminate();
    std::unique_ptr<NodeManager::SlaveStartupReply>
    HandleSlaveStartup(const NodeManager::SlaveStartupRequest& request);
    std::unique_ptr<NodeManager::SlaveConfiguredReply>
    HandleSlaveConfigured(const NodeManager::SlaveConfiguredRequest& request);
    std::unique_ptr<NodeManager::SlaveTerminatedReply>
    HandleSlaveTerminated(const NodeManager::SlaveTerminatedRequest& request);
    std::unique_ptr<NodeManager::SlaveHeartbeatReply>
    HandleSlaveHeartbeat(const NodeManager::SlaveHeartbeatRequest& request);

    auto Log(const std::vector<std::string>& log_messages) -> void;

    void HandleExperimentUpdate(const NodeManager::EnvironmentMessage& environment_update);

    // REVIEW (Mitch): Passing futures feels like a code smell.
    void ExecutionLoop(const sem::types::Timeout& duration,
                       std::future<void> execute_future,
                       std::future<void> terminate_future);

    // REVIEW (Mitch): Should investigate making these functions sync instead of async push.
    //  Require a slave to respond with success or failure upon acting receiving a
    //  controlmessage
    void PushControlMessage(const std::string& topic,
                            std::unique_ptr<NodeManager::ControlMessage> message);
    static std::unique_ptr<NodeManager::ControlMessage>
    ConstructStateControlMessage(NodeManager::ControlMessage::Type type);

    // REVIEW (Mitch): This comment scares me as none of these functions internally acquire said
    //  mutex
    // These need slave_state_mutex_ Mutex
    SlaveState GetSlaveState(const std::string& slave_key);
    void SetSlaveState(const std::string& slave_key, SlaveState state);
    int GetSlaveStateCount(const SlaveState& state);
    void SetSlaveAlive(const std::string& slave_key);

    // REVIEW (Mitch): What's the difference between slavestate and slavestatus?
    SlaveStatus& GetSlaveStatus(const std::string& slave_key);

    void HandleSlaveStateChange();

    Experiment& experiment_;
    const sem::types::Uuid experiment_manager_uuid_;
    const sem::types::SocketAddress master_publisher_endpoint_;
    const sem::types::SocketAddress master_registration_endpoint_;
    const sem::types::SocketAddress experiment_logger_endpoint_;


    std::mutex execution_mutex_;
    std::future<void> execution_future_;

    std::promise<void> execute_promise_;
    bool started_ = false;
    std::promise<void> terminate_promise_;

    std::mutex slave_state_mutex_;
    std::condition_variable slave_state_cv_;
    std::unordered_map<std::string, SlaveStatus> slave_status_;
    std::unordered_map<std::string, SlaveStatus> late_joiner_slave_status_;
    // REVIEW (Mitch): use atomic bool here?
    bool execution_valid_ = false;

    std::mutex control_message_mutex_;
    std::unique_ptr<NodeManager::ControlMessage> control_message_;
    std::unique_ptr<::zmq::ProtoWriter> proto_writer_;

    std::unique_ptr<::zmq::ProtoReplier> slave_registration_handler_;

    std::unique_ptr<Logan::ExperimentLogger> experiment_logger_;
};

} // namespace re::EnvironmentManager
#endif // EXECUTIONMANAGER_H
