#ifndef EXECUTIONMANAGER_H
#define EXECUTIONMANAGER_H

#include <memory>
#include <string>
#include <mutex>
#include <unordered_map>

#include <comms/environmentrequester/environmentrequester.h>
#include <proto/controlmessage/controlmessage.pb.h>
#include <zmq/protowriter/protowriter.h>
#include <zmq/protoreplier/protoreplier.hpp>
#include <util/execution.hpp>
#include <core/loggers/experiment_logger.h>

// REVIEW (Mitch): Remove forward decl
namespace zmq{class ProtoWriter;};

// REVIEW (Mitch): DOCUMENTATION: Exactly one of these is started per experiment.
//  Started iff the instance of the nodemanager executable is told by the environment manager that
//  it will be the master.
class ExecutionManager{
    public:
        // REVIEW (Mitch): no reason for this enum to be public
        enum class SlaveState{
            OFFLINE = 0,
            REGISTERED = 1,
            CONFIGURED = 2,
            TERMINATED = 3,
            ERROR_ = 4
        };

        // REVIEW (Mitch): no reason for this struct to be public
        struct SlaveStatus{
            SlaveState state;
            std::chrono::steady_clock::time_point heartbeat_time;
        };

        // REVIEW (Mitch): Use endpoint class
        ExecutionManager(Execution& execution,
                            int duration_sec,
                            const std::string& experiment_name,
                            const std::string& master_publisher_endpoint,
                            const std::string& master_registration_endpoint,
                            const std::string& master_heartbeat_endpoint,
                            const std::string& experiment_logger_endpoint
                            );
        ~ExecutionManager();
    private:
        static const std::string GetSlaveKey(const std::string& ip, const std::string& container_id);
        static const std::string GetSlaveKey(const NodeManager::SlaveId& slave);
        bool AllSlavesTerminated();
        bool IsSlaveNeeded(const std::string& slave_key);
        void RequestDeployment();
        void Terminate();
        std::unique_ptr<NodeManager::SlaveStartupReply> HandleSlaveStartup(const NodeManager::SlaveStartupRequest& request);
        std::unique_ptr<NodeManager::SlaveConfiguredReply> HandleSlaveConfigured(const NodeManager::SlaveConfiguredRequest& request);
        std::unique_ptr<NodeManager::SlaveTerminatedReply> HandleSlaveTerminated(const NodeManager::SlaveTerminatedRequest& request);
        std::unique_ptr<NodeManager::SlaveHeartbeatReply> HandleSlaveHeartbeat(const NodeManager::SlaveHeartbeatRequest request);


        void HandleExperimentUpdate(const NodeManager::EnvironmentMessage& environment_update);

        // REVIEW (Mitch): Passing futures feels like a code smell.
        void ExecutionLoop(int duration_sec, std::future<void> execute_future, std::future<void> terminate_future);

        // REVIEW (Mitch): Should investigate making these functions sync instead of async push.
        //  Require a slave to respond with success or failure upon acting receiving a
        //  controlmessage
        void PushControlMessage(const std::string& topic, std::unique_ptr<NodeManager::ControlMessage> message);
        void PushStateChange(const NodeManager::ControlMessage::Type& state);
        static std::unique_ptr<NodeManager::ControlMessage> ConstructStateControlMessage(NodeManager::ControlMessage::Type type);

        // REVIEW (Mitch): This comment scares me as none of these functions internally acquire said
        //  mutex
        //These need slave_state_mutex_ Mutex
        SlaveState GetSlaveState(const std::string& slave_key);
        void SetSlaveState(const std::string& slave_key, SlaveState state);
        int GetSlaveStateCount(const SlaveState& state);
        void SetSlaveAlive(const std::string& slave_key);

        // REVIEW (Mitch): What's the difference between slavestate and slavestatus?
        SlaveStatus& GetSlaveStatus(const std::string& slave_key);
        
        void HandleSlaveStateChange();

        const std::string experiment_name_;
        const std::string master_ip_addr_; // REVIEW (Mitch): unused
        const std::string master_publisher_endpoint_;
        const std::string master_registration_endpoint_;
        const std::string environment_manager_endpoint_; // REVIEW (Mitch): unused
        const std::string experiment_logger_endpoint_;

        std::mutex execution_mutex_;
        std::future<void> execution_future_;
        
        std::promise<void> execute_promise_;
        std::promise<void> terminate_promise_;



        std::mutex slave_state_mutex_;
        std::condition_variable slave_state_cv_;
        std::unordered_map<std::string, SlaveStatus> slave_status_;
        std::unordered_map<std::string, SlaveStatus> late_joiner_slave_status_;
        // REVIEW (Mitch): use atomic bool here?
        bool execution_valid_ = false;

        std::mutex control_message_mutex_;
        std::unique_ptr<NodeManager::ControlMessage> control_message_;
        std::unique_ptr<zmq::ProtoWriter> proto_writer_;

        Execution& execution_;
        std::unique_ptr<zmq::ProtoReplier> slave_registration_handler_;
        std::unique_ptr<EnvironmentRequest::HeartbeatRequester> requester_;

        std::unique_ptr<Logan::ExperimentLogger> experiment_logger_;
};

#endif //EXECUTIONMANAGER_H
