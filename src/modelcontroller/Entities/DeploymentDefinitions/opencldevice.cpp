#include "opencldevice.h"
#include "../../edgekinds.h"

OpenCLDevice::OpenCLDevice(EntityFactory* factory) : Node(factory, NODE_KIND::OPENCL_DEVICE, "OpenCLDevice"){
	auto node_kind = NODE_KIND::OPENCL_DEVICE;
    QString kind_string = "OpenCLDevice";
    
    RegisterNodeKind(factory, node_kind, kind_string, [](){return new OpenCLDevice();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "is_available", QVariant::Bool, true);
    RegisterDefaultData(factory, node_kind, "memory_size", QVariant::Int, true);
    RegisterDefaultData(factory, node_kind, "clock_freq", QVariant::Int, true);
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "version", QVariant::String, true);
};

OpenCLDevice::OpenCLDevice():Node(NODE_KIND::OPENCL_DEVICE)
{
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT);
    setNodeType(NODE_TYPE::HARDWARE);
}

bool OpenCLDevice::canAdoptChild(Node* child)
{
    return false;
}

bool OpenCLDevice::canAcceptEdge(EDGE_KIND, Node *)
{ 
    return false;
}
