#include "periodicport.h"
#include "../environment.h"
#include "../experiment.h"


using namespace EnvironmentManager;

PeriodicPort::PeriodicPort(::EnvironmentManager::Component& parent, const NodeManager::Port& port) :
    ::EnvironmentManager::Port(parent, port){

    for(const auto& attribute : port.attributes()){
        if(attribute.info().name() == "frequency"){
            frequency_ = attribute.d();
        }
    }
}

PeriodicPort::~PeriodicPort(){
}


double PeriodicPort::GetFrequency() const{
    return frequency_;
}

void PeriodicPort::FillPortPb(NodeManager::Port& port_pb){
    auto frequency_pb = port_pb.add_attributes();
    frequency_pb->mutable_info()->set_name("frequency");
    frequency_pb->set_kind(NodeManager::Attribute::DOUBLE);
    frequency_pb->set_d(GetFrequency());
}
