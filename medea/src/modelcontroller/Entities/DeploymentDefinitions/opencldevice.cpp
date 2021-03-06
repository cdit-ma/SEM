#include "opencldevice.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::OPENCL_DEVICE;
const QString kind_string = "OpenCL Device";

void OpenCLDevice::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new OpenCLDevice(broker, is_temp_node);
    });
}

OpenCLDevice::OpenCLDevice(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setLabelFunctional(false);
    setNodeType(NODE_TYPE::HARDWARE);
    //setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::TARGET);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::IsAvailable, QVariant::Bool, ProtectedState::PROTECTED, false);
    broker.AttachData(this, KeyName::MemorySize, QVariant::Int, ProtectedState::PROTECTED, -1);
    broker.AttachData(this, KeyName::ClockFrequency, QVariant::Int, ProtectedState::PROTECTED, -1);
    broker.AttachData(this, KeyName::Type, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Version, QVariant::String, ProtectedState::PROTECTED);
}