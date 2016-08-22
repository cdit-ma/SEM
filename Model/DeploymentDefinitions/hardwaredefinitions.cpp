#include "hardwaredefinitions.h"

HardwareDefinitions::HardwareDefinitions():Node(Node::NK_HARDWARE_DEFINITIONS)
{
}

VIEW_ASPECT HardwareDefinitions::getViewAspect()
{
    return VA_HARDWARE;
}

bool HardwareDefinitions::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NK_HARDWARE_CLUSTER:
    case NK_HARDWARE_NODE:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool HardwareDefinitions::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
