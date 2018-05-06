#include "hardwarenode.h"

#include "../../edgekinds.h"

auto node_kind = NODE_KIND::HARDWARE_NODE;
QString kind_string = "HardwareNode";

HardwareNode::HardwareNode(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new HardwareNode();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "architecture", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "ip_address", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "os", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "os_version", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "url", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "uuid", QVariant::String, true);
};

HardwareNode::HardwareNode():Node(node_kind)
{
    setNodeType(NODE_TYPE::HARDWARE);
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::TARGET);
    setAcceptsNodeKind(NODE_KIND::OPENCL_PLATFORM);
    setLabelFunctional();
}