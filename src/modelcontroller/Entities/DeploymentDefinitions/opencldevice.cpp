#include "opencldevice.h"
#include "../../entityfactory.h"

auto node_kind = NODE_KIND::OPENCL_DEVICE;
QString kind_string = "OpenCLDevice";

void OpenCLDevice::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new OpenCLDevice(factory, is_temp_node);
    });
}

OpenCLDevice::OpenCLDevice(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::HARDWARE);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::TARGET);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "", true);
    factory.AttachData(this, "is_available", QVariant::Bool, false, true);
    factory.AttachData(this, "memory_size", QVariant::Int, -1, true);
    factory.AttachData(this, "clock_freq", QVariant::Int, -1, true);
    factory.AttachData(this, "type", QVariant::String, "", true);
    factory.AttachData(this, "version", QVariant::String, "", true);
}