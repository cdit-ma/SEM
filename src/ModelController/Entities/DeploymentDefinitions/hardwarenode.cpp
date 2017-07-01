#include "hardwarenode.h"

#include "../../edgekinds.h"

HardwareNode::HardwareNode(EntityFactory* factory) : Node(factory, NODE_KIND::HARDWARE_NODE, "HardwareNode"){
	auto node_kind = NODE_KIND::HARDWARE_NODE;
	QString kind_string = "HardwareNode";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new HardwareNode();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "architecture", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "ip_address", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "os", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "os_version", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "url", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "uuid", QVariant::String, true);
};

HardwareNode::HardwareNode():Node(NODE_KIND::HARDWARE_NODE)
{
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT);
    setNodeType(NODE_TYPE::HARDWARE);

    //setMoveEnabled(false);
    //setExpandEnabled(false);
}

bool HardwareNode::canAdoptChild(Node*)
{
    return false;
}

bool HardwareNode::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
