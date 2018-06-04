#ifndef DDS_DEPLOYMENTRULE_H
#define DDS_DEPLOYMENTRULE_H

#include "../../deploymentrule.h"

namespace Dds{
    class DeploymentRule : public ::DeploymentRule{
        public:
            DeploymentRule(Environment& environment);

            void ConfigureEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port);
            void TerminateEventPort(const NodeManager::ControlMessage& message, NodeManager::Port& event_port);
    };
}

#endif //DDS_DEPLOYMENTRULE_H
