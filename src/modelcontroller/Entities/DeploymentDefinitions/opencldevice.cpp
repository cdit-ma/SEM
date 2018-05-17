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
    broker.AttachData(this, "label", QVariant::String, "", true);
    broker.AttachData(this, "is_available", QVariant::Bool, false, true);
    broker.AttachData(this, "memory_size", QVariant::Int, -1, true);
    broker.AttachData(this, "clock_freq", QVariant::Int, -1, true);
    broker.AttachData(this, "type", QVariant::String, "", true);
    broker.AttachData(this, "version", QVariant::String, "", true);
}