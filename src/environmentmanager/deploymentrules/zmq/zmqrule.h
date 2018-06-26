#ifndef ZMQ_DEPLOYMENTRULE_H
#define ZMQ_DEPLOYMENTRULE_H

#include "../../deploymentrule.h"

namespace Zmq{
    class DeploymentRule : public ::DeploymentRule{
        public:
            DeploymentRule(EnvironmentManager::Environment& environment);

            void ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port);
            void TerminateEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port);
    };
}

#endif //ZMQ_DEPLOYMENTRULE_H
