#include "periodicport.h"
#include "../environment.h"
#include "../experiment.h"
#include "helper.h"

namespace re::EnvironmentManager {

PeriodicPort::PeriodicPort(Component& parent, const NodeManager::Port& port) :
    EnvironmentManager::Port(parent, port)
{
    const auto& attribute = NodeManager::GetAttribute(port.attributes(), "Frequency");
    frequency_ = attribute.d();
}

PeriodicPort::PeriodicPort(const sem::types::Uuid event_uuid,
                           Component& parent,
                           const Representation::ExperimentDefinition& experiment_definition,
                           const Representation::PeriodicPortInstance& periodic_port_instance) :
    EnvironmentManager::Port(event_uuid, parent, experiment_definition, periodic_port_instance)
{
    frequency_ =
        experiment_definition.GetPeriodicPortFrequencyAttribute(periodic_port_instance);
}

double PeriodicPort::GetFrequency() const
{
    return frequency_;
}

void PeriodicPort::FillPortPb(NodeManager::Port& port_pb)
{
    NodeManager::SetDoubleAttribute(port_pb.mutable_attributes(), "frequency", GetFrequency());
}
} // namespace re::EnvironmentManager
