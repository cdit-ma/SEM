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
#include "executionparser/protobufmodelparser.h"
#include "environmentmanager/environment.h"
#include <re_common/zmq/environmentrequester/environmentrequester.h>

namespace zmq{class ProtoWriter;};
namespace Graphml{class ModelParser;};

class Execution;

class ExecutionManager{
    public:
        enum class SlaveState{
            OFFLINE = 0,
            ONLINE = 1,
            ERROR_ = 2,
        };

        ExecutionManager(const std::string& endpoint, const std::string& graphml_path, double execution_duration,
                            Execution* execution, const std::string& experiment_id, const std::string& environment_manager_endpoint = "");

        std::vector<std::string> GetSlaveAddresses();
        const NodeManager::SlaveStartup GetSlaveStartupMessage(const std::string& slave_ip);

        bool HandleSlaveResponseMessage(const NodeManager::SlaveStartupResponse& response);
        
        bool IsValid();
    private:
        void ExecutionLoop(double duration_sec) noexcept;

        void ActivateExecution();
        void TerminateExecution();

        int GetSlaveStateCount(const SlaveState& state);
        void PushMessage(const std::string& topic, google::protobuf::MessageLite* message);
        bool Finished();
    private:
        void TriggerExecution(bool execute);

        std::string GetSlaveHostName(const std::string& slave_ip);

        bool ConstructControlMessages();
        void ConfigureNode(const NodeManager::Node& node);
        bool PopulateDeployment();

        std::mutex mutex_;
        
        std::string master_ip_addr_;
        std::string master_publisher_endpoint_;
        std::string experiment_id_;
        std::string environment_manager_endpoint_;

        std::thread* execution_thread_ = 0;

        NodeManager::ControlMessage* deployment_message_;

        std::unordered_map<std::string, NodeManager::Node> deployment_map_;

        std::mutex execution_mutex_;
        std::condition_variable execution_lock_condition_;
        bool terminate_flag_ = false;

        bool local_mode_ = false;

        bool finished_ = false;
        bool parse_succeed_ = false;

        Execution* execution_;
        std::unique_ptr<EnvironmentRequester> requester_;
        
        zmq::ProtoWriter* proto_writer_;
        std::unique_ptr<ProtobufModelParser> protobuf_model_parser_;

        std::unordered_map<std::string, SlaveState> slave_states_;
};

#endif //EXECUTIONMANAGER_H
