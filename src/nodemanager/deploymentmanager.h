#ifndef DEPLOYMENTMANAGER_H
#define DEPLOYMENTMANAGER_H

#include <functional>
#include <string>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include <map>
#include <memory>

#include <core/modellogger.h>
#include "deploymentcontainer.h"

#include "controlmessage/controlmessage.pb.h"

namespace zmq{class ProtoReceiver;};
//namespace google{namespace protobuf{class MessageLite;}};
//namespace NodeManager{class ControlMessage;};
class Execution;

class DeploymentManager{
    public:
        DeploymentManager(std::string library_path, Execution* execution);
        ~DeploymentManager();

        bool SetupControlMessageReceiver(std::string pub_endpoint, std::string host_name);
        bool SetupModelLogger(std::string pub_endpoint, std::string host_name, ModelLogger::Mode mode);
        bool TeardownModelLogger();

        bool ProcessStartupMessage(const std::string& startup_str);
    private:
        std::shared_ptr<DeploymentContainer> GetDeploymentContainer(const std::string& node_name);
        void GotControlMessage(const NodeManager::ControlMessage& control_message);
        bool ConfigureDeploymentContainer(const NodeManager::ControlMessage& control_message);
        void InteruptQueueThread();
        void ProcessControlQueue();

        zmq::ProtoReceiver* subscriber_ = 0;

        std::string library_path_;
        
        Execution* execution_;

        std::map<std::string, std::shared_ptr<DeploymentContainer> > deployment_containers_;


        std::queue<NodeManager::ControlMessage> control_message_queue_;
        std::mutex mutex_;
        bool terminate_ = false;
        std::mutex notify_mutex_;
        
        std::thread* control_queue_thread_ = 0;
        std::condition_variable notify_lock_condition_;
};

#endif //DEPLOYMENTMANAGER_H