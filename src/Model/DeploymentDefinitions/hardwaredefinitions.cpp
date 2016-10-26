#include "hardwaredefinitions.h"

HardwareDefinitions::HardwareDefinitions():Node(Node::NK_HARDWARE_DEFINITIONS)
{
    setNodeType(NT_ASPECT);
}

VIEW_ASPECT HardwareDefinitions::getViewAspect() const
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

bool HardwareDefinitions::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
