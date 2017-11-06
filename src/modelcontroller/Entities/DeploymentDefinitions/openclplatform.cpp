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
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT);
    setNodeType(NODE_TYPE::HARDWARE);
}

bool OpenCLPlatform::canAdoptChild(Node* child)
{
    if(child->getNodeKind() != NODE_KIND::OPENCL_DEVICE){
        return false;
    }
    return Node::canAdoptChild(child);
}

bool OpenCLPlatform::canAcceptEdge(EDGE_KIND, Node *)
{ 
    return false;
}
