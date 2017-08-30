#ifndef DEPLOYMENTMANAGER_H
#define DEPLOYMENTMANAGER_H

#include <functional>
#include <string>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>

#include "nodecontainer.h"
#include "../modellogger.h"

namespace zmq{class ProtoReceiver;};
namespace google{namespace protobuf{class MessageLite;}};
namespace NodeManager{class ControlMessage;};
class Execution;

class DeploymentManager{
    public:
        DeploymentManager(std::string library_path, Execution* execution);
        ~DeploymentManager();

        bool SetupControlMessageReceiver(std::string pub_endpoint, std::string host_name);
        bool SetupModelLogger(std::string pub_endpoint, std::string host_name, ModelLogger::Mode mode);
        bool TeardownModelLogger();

        void GotControlMessage(google::protobuf::MessageLite* ml);
        NodeContainer* get_deployment();

    private:
        void ProcessControlQueue();

        zmq::ProtoReceiver* subscriber_ = 0;

        void Terminate();

        std::string library_path_;
        
        Execution* execution_;

        NodeContainer* deployment_ = 0;

        std::queue<NodeManager::ControlMessage*> control_message_queue_;
        std::mutex mutex_;
        std::mutex notify_mutex_;
        
        std::thread* control_queue_thread_ = 0;
        std::condition_variable notify_lock_condition_;
};

#endif //DEPLOYMENTMANAGER_H