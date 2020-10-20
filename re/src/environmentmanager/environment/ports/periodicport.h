#ifndef ENVIRONMENT_MANAGER_PERIODIC_PORT_H
#define ENVIRONMENT_MANAGER_PERIODIC_PORT_H

#include <mutex>
#include <memory>
#include "controlmessage.pb.h"

#include "port.h"

namespace re::EnvironmentManager{
class Environment;
class Experiment;
class Node;
class Component;
class Attribute;
class PeriodicPort : public EnvironmentManager::Port{
    public:
        PeriodicPort(Component& parent, const NodeManager::Port& port);
        PeriodicPort(const types::Uuid event_uuid,
                     Component& parent,
                     const Representation::ExperimentDefinition& experiment_definition,
                     const Representation::PeriodicPortInstance& periodic_port_instance);
        ~PeriodicPort() final = default;
    protected:
        double GetFrequency() const;
        void FillPortPb(NodeManager::Port& port_pb) final;

    private:
        double frequency_ = 1;
};
};

#endif //ENVIRONMENT_MANAGER_PERIODIC_PORT_H