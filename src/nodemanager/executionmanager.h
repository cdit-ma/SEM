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
            CONFIGURED = 1,
            TERMINATED = 2,
            ERROR_ = 3
        };

        ExecutionManager(Execution& execution,
                            int duration_sec,
                            const std::string& experiment_name,
                            const std::string& master_publisher_endpoint,
                            const std::string& master_registration_endpoint,
                            const std::string& master_heartbeat_endpoint);
        ~ExecutionManager();
    private:
        void RequestDeployment();
        void Terminate();
        std::unique_ptr<NodeManager::SlaveStartupReply> HandleSlaveStartup(const NodeManager::SlaveStartupRequest& request);
        std::unique_ptr<NodeManager::SlaveConfiguredReply> HandleSlaveConfigured(const NodeManager::SlaveConfiguredRequest& request);
        std::unique_ptr<NodeManager::SlaveTerminatedReply> HandleSlaveTerminated(const NodeManager::SlaveTerminatedRequest& request);
        void HandleExperimentUpdate(const NodeManager::EnvironmentMessage& environment_update);
        
        void ExecutionLoop(int duration_sec, std::future<void> execute_future, std::future<void> terminate_future, std::future<void> slave_deregistration_future);
        
        void PushControlMessage(const std::string& topic, std::unique_ptr<NodeManager::ControlMessage> message);
        static std::unique_ptr<NodeManager::ControlMessage> ConstructStateControlMessage(NodeManager::ControlMessage::Type type);

        //These need slave_state_mutex_ Mutex
        SlaveState GetSlaveState(const std::string& ip_address);
        void SetSlaveState(const std::string& ip_address, SlaveState state);
        int GetSlaveStateCount(const SlaveState& state);
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
        std::promise<void> slave_deregistration_promise_;


        std::mutex slave_state_mutex_;
        std::unordered_map<std::string, SlaveState> slave_states_;
        bool execution_valid_ = false;

        std::mutex control_message_mutex_;
        std::unique_ptr<NodeManager::ControlMessage> control_message_;
        std::unique_ptr<zmq::ProtoWriter> proto_writer_;

        Execution& execution_;
        std::unique_ptr<zmq::ProtoReplier> slave_registration_handler_;
        std::unique_ptr<EnvironmentRequest::NodeManagerHeartbeatRequester> requester_;
};

#endif //EXECUTIONMANAGER_H
