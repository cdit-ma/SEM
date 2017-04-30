#include "hardwarenode.h"

HardwareNode::HardwareNode():Node(NODE_KIND::HARDWARE_NODE)
{
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
    setNodeType(NODE_TYPE::HARDWARE);

    setMoveEnabled(false);
    setExpandEnabled(false);
    
    updateDefaultData("architecture", QVariant::String, true);
    updateDefaultData("ip_address", QVariant::String, true);
    updateDefaultData("os", QVariant::String, true);
    updateDefaultData("os_version", QVariant::String, true);
}

bool HardwareNode::canAdoptChild(Node*)
{
    return false;
}

bool HardwareNode::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
