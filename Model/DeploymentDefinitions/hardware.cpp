#include "hardware.h"

Hardware::Hardware(bool isHardwareNode):Node(Node::NT_HARDWARE)
{
    hardwareNode = isHardwareNode;

    setAcceptsEdgeClass(Edge::EC_DEPLOYMENT);
}

bool Hardware::isHardwareNode()
{
    return hardwareNode;
}

bool Hardware::isHardwareCluster()
{
    return !hardwareNode;
}

bool Hardware::canAdoptChild(Node *node)
{
    return Node::canAdoptChild(node);
}

