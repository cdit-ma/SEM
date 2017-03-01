#ifndef DEPLOYMENTMANAGER_H
#define DEPLOYMENTMANAGER_H

#include <functional>
#include <string>
#include <mutex>

#include "nodecontainer.h"

namespace zmq{class ProtoReceiver;};
namespace google{namespace protobuf{class MessageLite;}};


class DeploymentManager{
    public:
        DeploymentManager(std::string library_path);
        ~DeploymentManager();

        bool SetupControlMessageReceiver(std::string pub_endpoint, std::string host_name);
        bool SetupModelLogger(std::string pub_endpoint, std::string host_name);

        void ProcessControlMessage(google::protobuf::MessageLite* ml);
        void ProcessAction(std::string node_name, std::string action);

        NodeContainer* get_deployment();

    private:
        zmq::ProtoReceiver* subscriber_ = 0;


        std::string library_path_;
        std::mutex mutex_;
        

        NodeContainer* deployment_ = 0;
};

#endif //DEPLOYMENTMANAGER_H