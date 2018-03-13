#ifndef ZMQ_DEPLOYMENTRULE_H
#define ZMQ_DEPLOYMENTRULE_H

#include "../../deploymentrule.h"

namespace Zmq{
    class DeploymentRule : public ::DeploymentRule{
        public:
            DeploymentRule();

            void ConfigureEventPort(const NodeManager::EnvironmentMessage& message, NodeManager::EventPort& event_port);
            void TerminateEventPort(const NodeManager::EnvironmentMessage& message, NodeManager::EventPort& event_port);
    };
}

#endif //DEPLOYMENTRULE_H
