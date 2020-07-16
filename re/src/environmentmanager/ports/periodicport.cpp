#include "periodicport.h"
#include "../environment.h"
#include "../experiment.h"
#include "helper.h"

using namespace EnvironmentManager;

PeriodicPort::PeriodicPort(::EnvironmentManager::Component& parent, const NodeManager::Port& port) :
    ::EnvironmentManager::Port(parent, port){

    const auto& attribute = NodeManager::GetAttribute(port.attributes(), "Frequency");
    frequency_ = attribute.d();
}

// REVIEW (Mitch): =default
PeriodicPort::~PeriodicPort(){
}


double PeriodicPort::GetFrequency() const{
    return frequency_;
}

void PeriodicPort::FillPortPb(NodeManager::Port& port_pb){
    NodeManager::SetDoubleAttribute(port_pb.mutable_attributes(), "frequency", GetFrequency());
}
