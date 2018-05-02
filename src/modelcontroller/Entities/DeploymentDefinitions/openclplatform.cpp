#include "openclplatform.h"
#include "../../edgekinds.h"

OpenCLPlatform::OpenCLPlatform(EntityFactory* factory) : Node(factory, NODE_KIND::OPENCL_PLATFORM, "OpenCLPlatform"){
	auto node_kind = NODE_KIND::OPENCL_PLATFORM;
    QString kind_string = "OpenCLPlatform";
    
    RegisterNodeKind(factory, node_kind, kind_string, [](){return new OpenCLPlatform();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "vendor", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "version", QVariant::String, true);
};

OpenCLPlatform::OpenCLPlatform():Node(NODE_KIND::OPENCL_PLATFORM)
{
    setNodeType(NODE_TYPE::HARDWARE);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::TARGET);
    setAcceptsNodeKind(NODE_KIND::OPENCL_DEVICE);
}