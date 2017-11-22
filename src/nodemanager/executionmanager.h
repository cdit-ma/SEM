#ifndef EXECUTIONMANAGER_H
#define EXECUTIONMANAGER_H

#include <mutex>
#include <thread>
#include <condition_variable>
#include <string>
#include <iostream>
#include <map>
#include <vector>

namespace google{
    namespace protobuf{
        class MessageLite;
    };
};

namespace NodeManager{
    class ControlMessage;
};

namespace zmq{class ProtoWriter;};
namespace Graphml{class ModelParser;};

class Execution;

class ExecutionManager{
    public:
        ExecutionManager(const std::string& endpoint, const std::string& graphml_path, double execution_duration, Execution* execution);

        std::vector<std::string> GetNodeManagerSlaveAddresses();
        
        std::string GetNodeNameFromNodeManagerAddress(const std::string& address);
        std::string GetModelLoggerAddressFromNodeName(const std::string& host_name);
        std::string GetModelLoggerModeFromNodeName(const std::string& host_name);

        void ExecutionLoop(double duration_sec);

        void ActivateExecution();
        void TerminateExecution();

        void PushMessage(const std::string& topic, google::protobuf::MessageLite* message);
        void SlaveOnline(const std::string& response, const std::string& endpoint, const std::string& host_name);

        std::string GetSlaveStartupMessage(const std::string& slave_host_name);
        bool Finished();
        bool IsValid();
        
        std::vector<NodeManager::ControlMessage*> getNodeStartupMessage();
    private:
        
        void HandleSlaveOnline(const std::string& endpoint);
        bool ConstructControlMessages();

        std::mutex mutex_;

        std::thread* execution_thread_ = 0;

        std::map<std::string, NodeManager::ControlMessage*> deployment_map_;

        std::mutex activate_mutex_;
        std::condition_variable activate_lock_condition_;

        std::mutex terminate_mutex_;
        std::condition_variable terminate_lock_condition_;
        bool terminate_flag_ = false;
        bool finished_ = false;
        bool parse_succeed_ = false;

        Execution* execution_;
        
        zmq::ProtoWriter* proto_writer_;
        Graphml::ModelParser* model_parser_;

        std::vector<std::string> required_slave_addresses_;
        std::vector<std::string> inactive_slave_addresses_;
};

#endif //EXECUTIONMANAGER_H