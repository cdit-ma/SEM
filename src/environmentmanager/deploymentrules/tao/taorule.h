#ifndef TAO_DEPLOYMENTRULE_H
#define TAO_DEPLOYMENTRULE_H

#include "../../deploymentrule.h"

namespace Tao{
    class DeploymentRule : public ::DeploymentRule{
        public:
            DeploymentRule(Environment& environment);
            void ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port);
            void TerminateEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port);
    };
}

#endif //ZMQ_DEPLOYMENTRULE_H
