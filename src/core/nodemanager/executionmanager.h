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

class ExecutionManager{
    public:
        ExecutionManager(std::string endpoint, std::string graphml_path, double execution_duration);

        std::vector<std::string> GetNodeManagerSlaveAddresses();
        
        std::string GetNodeNameFromNodeManagerAddress(std::string address);
        std::string GetModelLoggerAddressFromNodeName(std::string host_name);

        void ExecutionLoop(double duration_sec);

        void ActivateExecution();
        void TerminateExecution();

        void PushMessage(std::string topic, google::protobuf::MessageLite* message);
        void SlaveOnline(std::string response, std::string endpoint, std::string host_name);
    private:
        void HandleSlaveOnline(std::string endpoint);
        bool ConstructControlMessages();

        std::mutex mutex_;

        std::thread* execution_thread_ = 0;

        std::map<std::string, NodeManager::ControlMessage*> deployment_map_;

        std::mutex activate_mutex_;
        std::condition_variable activate_lock_condition_;

        std::mutex terminate_mutex_;
        std::condition_variable terminate_lock_condition_;

        zmq::ProtoWriter* proto_writer_;
        Graphml::ModelParser* model_parser_;

        std::vector<std::string> required_slave_addresses_;
        std::vector<std::string> inactive_slave_addresses_;
};

#endif //EXECUTIONMANAGER_H