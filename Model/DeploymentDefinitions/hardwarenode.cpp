#include "hardwarenode.h"

HardwareNode::HardwareNode():Node(NK_HARDWARE_NODE)
{
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
}

bool HardwareNode::canAdoptChild(Node*)
{
    return false;
}

bool HardwareNode::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
