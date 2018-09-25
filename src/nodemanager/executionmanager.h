#ifndef EXECUTIONMANAGER_H
#define EXECUTIONMANAGER_H

#include <mutex>
#include <thread>
#include <condition_variable>
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <google/protobuf/message_lite.h>
#include "environmentmanager/environment.h"
#include <util/graphmlparser/protobufmodelparser.h>
#include <comms/environmentrequester/environmentrequester.h>
#include "zmq/registrar.h"

namespace zmq{class ProtoWriter;};

class Execution;

class ExecutionManager{
    public:
        enum class SlaveState{
            OFFLINE = 0,
            ONLINE = 1,
            ERROR_ = 2,
        };

        ExecutionManager(Execution& execution,
                            double execution_duration,
                            const std::string& experiment_name,
                            const std::string& master_publisher_endpoint,
                            const std::string& master_registration_endpoint,
                            const std::string& master_heartbeat_endpoint);

        std::vector<std::string> GetSlaveAddresses();
        const NodeManager::SlaveStartup GetSlaveStartupMessage(const std::string& slave_ip);

        void GotSlaveTerminated(const std::string& slave_ip);

        bool HandleSlaveResponseMessage(const NodeManager::SlaveStartupResponse& response);
        
        std::string GetMasterRegistrationEndpoint();
        bool IsValid();

    private:
        void ExecutionLoop(double duration_sec) noexcept;

        void ActivateExecution();
        void TerminateExecution();

        int GetSlaveStateCount(const SlaveState& state);
        
        void PushControlMessage(const std::string& topic, std::unique_ptr<NodeManager::ControlMessage> message);
        bool Finished();
    private:
        void ExperimentUpdate(const NodeManager::EnvironmentMessage& environment_update);
        int GetSlaveStateCountTS(const SlaveState& state);
        void TriggerExecution(bool execute);

        std::string GetSlaveHostName(const std::string& slave_ip);

        bool ConstructControlMessages();
        void ConfigureNode(const NodeManager::Node& node);
        bool PopulateDeployment();

        std::mutex mutex_;
        
        std::string master_ip_addr_;
        std::string master_publisher_endpoint_;
        std::string master_registration_endpoint_;
        std::string experiment_id_;
        std::string environment_manager_endpoint_;

        std::future<void> execution_future_;


        std::unique_ptr<zmq::Registrar> registrar_;
        std::unique_ptr<NodeManager::ControlMessage> deployment_message_;

        std::unique_ptr<NodeManager::ControlMessage> control_message_;

        std::unordered_map<std::string, NodeManager::Node> deployment_map_;

        std::mutex execution_mutex_;
        std::condition_variable execution_lock_condition_;
        bool terminate_flag_ = false;
        bool execute_flag_ = false;

        bool finished_ = false;
        bool parse_succeed_ = false;

        Execution& execution_;
        std::unique_ptr<EnvironmentRequest::NodeManagerHeartbeatRequester> requester_;
        
        zmq::ProtoWriter proto_writer_;

        std::mutex slave_state_mutex_;
        std::unordered_map<std::string, SlaveState> slave_states_;
        std::condition_variable slave_state_cv_;
};

#endif //EXECUTIONMANAGER_H
