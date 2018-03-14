#ifndef ZMQ_DEPLOYMENTRULE_H
#define ZMQ_DEPLOYMENTRULE_H

#include "../../deploymentrule.h"

namespace Zmq{
    class DeploymentRule : public ::DeploymentRule{
        public:
            DeploymentRule(Environment& environment);

            void ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::EventPort& event_port);
            void TerminateEventPort(const NodeManager::ControlMessage& message, NodeManager::EventPort& event_port);

        private:
            Environment& environment_;
    };
}

#endif //DEPLOYMENTRULE_H
