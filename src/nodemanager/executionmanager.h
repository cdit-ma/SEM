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
#include "environmentrequester.h"

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

        ExecutionManager(const std::string& endpoint, const std::string& graphml_path, double execution_duration,
                            Execution* execution, const std::string& environment_manager_endpoint = "");

        std::vector<std::string> GetSlaveAddresses();
        const NodeManager::Startup GetSlaveStartupMessage(const std::string& slave_address);
        bool HandleSlaveResponseMessage(const std::string& slave_address, const NodeManager::StartupResponse& response);
        
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
        void ConfigureNode(const NodeManager::Node& node);
        bool PopulateDeployment();

        std::mutex mutex_;
        std::string master_endpoint_;

        std::thread* execution_thread_ = 0;

        NodeManager::ControlMessage* deployment_message_;

        std::unordered_map<std::string, NodeManager::Node> deployment_map_;

        std::mutex execution_mutex_;
        std::condition_variable execution_lock_condition_;
        bool terminate_flag_ = false;

        bool local_mode_ = true;

        bool finished_ = false;
        bool parse_succeed_ = false;

        Execution* execution_;
        EnvironmentRequester* requester_ = 0;
        
        zmq::ProtoWriter* proto_writer_;
        Graphml::ModelParser* model_parser_;
        ProtobufModelParser* protobuf_model_parser_;

        
        std::unordered_map<std::string, SlaveState> slave_states_;
};

#endif //EXECUTIONMANAGER_H