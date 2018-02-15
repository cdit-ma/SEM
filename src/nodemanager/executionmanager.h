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

#include "controlmessage/controlmessage.pb.h"


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

        ExecutionManager(const std::string& endpoint, const std::string& graphml_path, double execution_duration, Execution* execution);

        std::vector<std::string> GetSlaveAddresses();
        const NodeManager::Startup GetSlaveStartupMessage(const std::string& slave_address);
        bool HandleSlaveResponseMessage(const std::string& slave_address, const NodeManager::StartupResponse& response);
        
        std::string GetSlaveNameFromAddress(const std::string& slave_address);
        std::vector<NodeManager::ControlMessage*> getNodeStartupMessage();
        bool IsValid();
    private:
        void ExecutionLoop(double duration_sec);

        void ActivateExecution();
        void TerminateExecution();

        int GetSlaveStateCount(const SlaveState& state);
        void PushMessage(const std::string& topic, google::protobuf::MessageLite* message);
        bool Finished();
    private:
        void TriggerExecution(bool execute);

        bool ConstructControlMessages();

        std::mutex mutex_;
        std::string master_endpoint_;

        std::thread* execution_thread_ = 0;

        std::unordered_map<std::string, NodeManager::ControlMessage*> deployment_map_;

        std::mutex execution_mutex_;
        std::condition_variable execution_lock_condition_;
        bool terminate_flag_ = false;
        
        bool finished_ = false;
        bool parse_succeed_ = false;

        Execution* execution_;
        
        zmq::ProtoWriter* proto_writer_;
        Graphml::ModelParser* model_parser_;

        
        std::unordered_map<std::string, SlaveState> slave_states_;
};

#endif //EXECUTIONMANAGER_H