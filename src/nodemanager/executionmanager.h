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

namespace zmq{class ProtoWriter;};

class ExecutionManager{
    public:
        enum class SlaveState{
            OFFLINE = 0,
            REGISTERED = 1,
            CONFIGURED = 2,
            TERMINATED = 3,
            ERROR_ = 4
        };

        struct SlaveStatus{
            SlaveState state;
            std::chrono::steady_clock::time_point heartbeat_time;
        };

        ExecutionManager(Execution& execution,
                            int duration_sec,
                            const std::string& experiment_name,
                            const std::string& master_publisher_endpoint,
                            const std::string& master_registration_endpoint,
                            const std::string& master_heartbeat_endpoint);
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
        
        void ExecutionLoop(int duration_sec, std::future<void> execute_future, std::future<void> terminate_future);
        
        void PushControlMessage(const std::string& topic, std::unique_ptr<NodeManager::ControlMessage> message);
        static std::unique_ptr<NodeManager::ControlMessage> ConstructStateControlMessage(NodeManager::ControlMessage::Type type);

        //These need slave_state_mutex_ Mutex
        SlaveState GetSlaveState(const std::string& slave_key);
        void SetSlaveState(const std::string& slave_key, SlaveState state);
        int GetSlaveStateCount(const SlaveState& state);
        void SetSlaveAlive(const std::string& slave_key);

        SlaveStatus& GetSlaveStatus(const std::string& slave_key);
        
        void HandleSlaveStateChange();

        std::string experiment_name_;
        std::string master_ip_addr_;
        std::string master_publisher_endpoint_;
        std::string master_registration_endpoint_;
        std::string environment_manager_endpoint_;

        std::mutex execution_mutex_;
        std::future<void> execution_future_;
        
        std::promise<void> execute_promise_;
        std::promise<void> terminate_promise_;



        std::mutex slave_state_mutex_;
        std::condition_variable slave_state_cv_;
        std::unordered_map<std::string, SlaveStatus> slave_status_;
        std::unordered_map<std::string, SlaveStatus> late_joiner_slave_status_;
        bool execution_valid_ = false;

        std::mutex control_message_mutex_;
        std::unique_ptr<NodeManager::ControlMessage> control_message_;
        std::unique_ptr<zmq::ProtoWriter> proto_writer_;

        Execution& execution_;
        std::unique_ptr<zmq::ProtoReplier> slave_registration_handler_;
        std::unique_ptr<EnvironmentRequest::HeartbeatRequester> requester_;
};

#endif //EXECUTIONMANAGER_H
