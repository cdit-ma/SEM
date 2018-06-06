#ifndef AMQP_DEPLOYMENTRULE_H
#define AMQP_DEPLOYMENTRULE_H

#include "../../deploymentrule.h"

namespace Amqp{
    class DeploymentRule : public ::DeploymentRule{
        public:
            DeploymentRule(Environment& environment);

            void ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port);
            void TerminateEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port);
    };
}

#endif //AMQP_DEPLOYMENTRULE_H
