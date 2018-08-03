#ifndef ENVIRONMENT_MANAGER_PERIODIC_PORT_H
#define ENVIRONMENT_MANAGER_PERIODIC_PORT_H

#include <mutex>
#include <memory>
#include <re_common/proto/controlmessage/controlmessage.pb.h>

#include "port.h"

namespace EnvironmentManager{
class Environment;
class Experiment;
class Node;
class Component;
class Attribute;
class PeriodicPort : public ::EnvironmentManager::Port{
    public:
        PeriodicPort(::EnvironmentManager::Component& parent, const NodeManager::Port& port);
        ~PeriodicPort();
    protected:
        double GetFrequency() const;
        void FillPortPb(NodeManager::Port& port_pb);

    private:
        double frequency_ = 1;
};
};

#endif //ENVIRONMENT_MANAGER_PERIODIC_PORT_H