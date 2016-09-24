#include "hardwarenode.h"

HardwareNode::HardwareNode():Node(NK_HARDWARE_NODE)
{
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
    setNodeType(NT_HARDWARE);
}

bool HardwareNode::canAdoptChild(Node*)
{
    return false;
}

bool HardwareNode::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
