#ifndef ZMQ_DEPLOYMENTRULE_H
#define ZMQ_DEPLOYMENTRULE_H

#include "../../deploymentrule.h"

namespace Zmq{
    class DeploymentRule : public ::DeploymentRule{
        public:
            DeploymentRule(Environment& environment);

            void ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port);
            void TerminateEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port);

        private:
            std::string GetDeploymentLocation(const NodeManager::ControlMessage& message, 
                                                            const std::string& port_id);
            std::string CheckNode(const NodeManager::Node& node, const std::string& port_id);
    };
}

#endif //ZMQ_DEPLOYMENTRULE_H
